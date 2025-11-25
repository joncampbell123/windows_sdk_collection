# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Outline - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to Outline - Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "Outline - Macintosh Release" && "$(CFG)" !=\
 "Outline - Macintosh Release (container)" && "$(CFG)" !=\
 "Outline - Macintosh Release (server)" && "$(CFG)" !=\
 "Outline - Macintosh Release (inplace container)" && "$(CFG)" !=\
 "Outline - Macintosh Release (inplace server)" && "$(CFG)" !=\
 "Outline - Macintosh Debug" && "$(CFG)" !=\
 "Outline - Macintosh Debug (container)" && "$(CFG)" !=\
 "Outline - Macintosh Debug (server)" && "$(CFG)" !=\
 "Outline - Macintosh Debug (inplace container)" && "$(CFG)" !=\
 "Outline - Macintosh Debug (inplace server)"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "outl.mak" CFG="Outline - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Outline - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Outline - Macintosh Release (container)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Release (server)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Release (inplace container)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Release (inplace server)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Outline - Macintosh Debug (container)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Debug (server)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Debug (inplace container)" (based on\
 "Macintosh Application")
!MESSAGE "Outline - Macintosh Debug (inplace server)" (based on\
 "Macintosh Application")
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
# PROP Target_Last_Scanned "Outline - Macintosh Debug (inplace container)"
MRC=mrc.exe
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir outl
# PROP Intermediate_Dir outl
OUTDIR=.\outl
INTDIR=.\outl

ALL : ".\bin\outlr.exe" ".\bin\outlr.trg"

CLEAN : 
	-@erase ".\bin\outlr.trg"
	-@erase ".\bin\outlr.exe"
	-@erase ".\outl\toolbar.obj"
	-@erase ".\outl\debug.obj"
	-@erase ".\outl\gopher.obj"
	-@erase ".\outl\util.obj"
	-@erase ".\outl\except.obj"
	-@erase ".\outl\pobj.obj"
	-@erase ".\outl\app.obj"
	-@erase ".\outl\main.obj"
	-@erase ".\outl\olexcept.obj"
	-@erase ".\outl\window.obj"
	-@erase ".\outl\line.obj"
	-@erase ".\outl\table.obj"
	-@erase ".\outl\doc.obj"
	-@erase ".\outl\oletypes.rsc"
	-@erase ".\outl\outline.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outl\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D "qOleMessageFilter" /D = /D "0" /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D "NDEBUG" /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleMessageFilter=0 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h"\
 /D "_MSC_VER" /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D\
 qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D "qOleMessageFilter" /D\
 = /D "0" /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D "NDEBUG"\
 /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 qIMacFrameTools=1 /D qOleMessageFilter=0 /D "_WINGS" /D "_InlineMake"\
 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c 
CPP_OBJS=.\outl/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG" /d "_MSC_VER"
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=0\
 /D qAssert=0 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OUTL" /machine:M68K /out:"bin\outlr.exe"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OUTL"\
 /pdb:"$(OUTDIR)/outlr.pdb" /machine:M68K /out:"bin\outlr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc"

".\bin\outlr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlr.exe"

".\bin\outlr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlr.exe "$(MFILE32_DEST)":outlr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlr.exe "$(MFILE32_DEST)":outlr.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OutlineC
# PROP BASE Intermediate_Dir OutlineC
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outc
# PROP Intermediate_Dir outc
OUTDIR=.\outc
INTDIR=.\outc

ALL : ".\bin\outlcd.exe" ".\bin\outlcd.trg"

CLEAN : 
	-@erase ".\outc\outlcd.pdb"
	-@erase ".\outc\toolbar.obj"
	-@erase ".\outc\line.obj"
	-@erase ".\outc\olexcept.obj"
	-@erase ".\outc\window.obj"
	-@erase ".\outc\site.obj"
	-@erase ".\outc\app.obj"
	-@erase ".\outc\appcntr.obj"
	-@erase ".\outc\util.obj"
	-@erase ".\outc\gopher.obj"
	-@erase ".\outc\dxferdoc.obj"
	-@erase ".\outc\except.obj"
	-@erase ".\outc\table.obj"
	-@erase ".\outc\doc.obj"
	-@erase ".\outc\pobj.obj"
	-@erase ".\outc\doccntr.obj"
	-@erase ".\outc\main.obj"
	-@erase ".\outc\debug.obj"
	-@erase ".\outc\containr.rsc"
	-@erase ".\outc\oletypes.rsc"
	-@erase ".\outc\outline.rsc"
	-@erase ".\bin\outlcd.trg"
	-@erase ".\bin\outlcd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outc\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c 
CPP_OBJS=.\outc/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0\
 /D qAssert=0 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oleldf.obj /nologo /MAC:bundle /MAC:creator="OUTC" /pdb:"objects/outlcd.pdb" /debug /machine:M68K /out:"apps\outlcd.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="OUTC" /debug /machine:M68K /out:"bin\outlcd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OUTC" /pdb:"$(OUTDIR)/outlcd.pdb" /debug\
 /machine:M68K /out:"bin\outlcd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/containr.rsc" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc"

".\bin\outlcd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlcd.exe"

".\bin\outlcd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlcd.exe "$(MFILE32_DEST)":outlcd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlcd.exe "$(MFILE32_DEST)":outlcd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Outline0
# PROP BASE Intermediate_Dir Outline0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outs
# PROP Intermediate_Dir outs
OUTDIR=.\outs
INTDIR=.\outs

ALL : ".\bin\outlsr.exe" ".\bin\outlsr.trg"

CLEAN : 
	-@erase ".\bin\outlsr.trg"
	-@erase ".\bin\outlsr.exe"
	-@erase ".\outs\gopher.obj"
	-@erase ".\outs\app.obj"
	-@erase ".\outs\util.obj"
	-@erase ".\outs\except.obj"
	-@erase ".\outs\toolbar.obj"
	-@erase ".\outs\dxferdoc.obj"
	-@erase ".\outs\pobj.obj"
	-@erase ".\outs\appsvr.obj"
	-@erase ".\outs\table.obj"
	-@erase ".\outs\doc.obj"
	-@erase ".\outs\main.obj"
	-@erase ".\outs\window.obj"
	-@erase ".\outs\debug.obj"
	-@erase ".\outs\line.obj"
	-@erase ".\outs\docsvr.obj"
	-@erase ".\outs\olexcept.obj"
	-@erase ".\outs\outline.rsc"
	-@erase ".\outs\oletypes.rsc"
	-@erase ".\outs\server.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outs\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fo"$(INTDIR)/" /Q68000 /c 
CPP_OBJS=.\outs/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_MAC" /D\
 qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0\
 /D qAssert=0 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oleldf.obj /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 olelrf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="OUTS" /machine:M68K /out:"bin\outlsr.exe"
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=olelrf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OUTS" /pdb:"$(OUTDIR)/outlsr.pdb" /machine:M68K\
 /out:"bin\outlsr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/server.rsc"

".\bin\outlsr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlsr.exe"

".\bin\outlsr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlsr.exe "$(MFILE32_DEST)":outlsr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlsr.exe "$(MFILE32_DEST)":outlsr.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Outline0
# PROP BASE Intermediate_Dir Outline0
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outipc
# PROP Intermediate_Dir outipc
OUTDIR=.\outipc
INTDIR=.\outipc

ALL : ".\bin\outlipcd.exe" ".\bin\outlipcd.trg"

CLEAN : 
	-@erase ".\outipc\outlipcd.pdb"
	-@erase ".\outipc\doc.obj"
	-@erase ".\outipc\doccntr.obj"
	-@erase ".\outipc\pobj.obj"
	-@erase ".\outipc\window.obj"
	-@erase ".\outipc\toolbar.obj"
	-@erase ".\outipc\main.obj"
	-@erase ".\outipc\dxferdoc.obj"
	-@erase ".\outipc\line.obj"
	-@erase ".\outipc\gopher.obj"
	-@erase ".\outipc\table.obj"
	-@erase ".\outipc\site.obj"
	-@erase ".\outipc\app.obj"
	-@erase ".\outipc\appcntr.obj"
	-@erase ".\outipc\except.obj"
	-@erase ".\outipc\debug.obj"
	-@erase ".\outipc\olexcept.obj"
	-@erase ".\outipc\util.obj"
	-@erase ".\outipc\containr.rsc"
	-@erase ".\outipc\outline.rsc"
	-@erase ".\outipc\oletypes.rsc"
	-@erase ".\bin\outlipcd.trg"
	-@erase ".\bin\outlipcd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outipc\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D qDebug=1 /D qAssert=1 /YX /Q68000 /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D qDebug=0 /D qAssert=0 /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c 
CPP_OBJS=.\outipc/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "NDEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=1 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0\
 /D qAssert=0 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oleldf.obj /nologo /MAC:bundle /MAC:creator="OTIC" /pdb:"objects/outlipcd.pdb" /debug /machine:M68K /out:"apps\outlipcd.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="OTIC" /debug /machine:M68K /out:"bin\outlipcd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OTIC" /pdb:"$(OUTDIR)/outlipcd.pdb" /debug\
 /machine:M68K /out:"bin\outlipcd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/containr.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/oletypes.rsc"

".\bin\outlipcd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlipcd.exe"

".\bin\outlipcd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipcd.exe\
 "$(MFILE32_DEST)":outlipcd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipcd.exe\
 "$(MFILE32_DEST)":outlipcd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OutlineI
# PROP BASE Intermediate_Dir OutlineI
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outips
# PROP Intermediate_Dir outips
OUTDIR=.\outips
INTDIR=.\outips

ALL : ".\bin\outlipsd.exe" ".\bin\outlipsd.trg"

CLEAN : 
	-@erase ".\bin\outlipsd.trg"
	-@erase ".\bin\outlipsd.exe"
	-@erase ".\outips\docsvr.obj"
	-@erase ".\outips\gopher.obj"
	-@erase ".\outips\main.obj"
	-@erase ".\outips\dxferdoc.obj"
	-@erase ".\outips\line.obj"
	-@erase ".\outips\except.obj"
	-@erase ".\outips\table.obj"
	-@erase ".\outips\app.obj"
	-@erase ".\outips\appsvr.obj"
	-@erase ".\outips\util.obj"
	-@erase ".\outips\debug.obj"
	-@erase ".\outips\olexcept.obj"
	-@erase ".\outips\toolbar.obj"
	-@erase ".\outips\window.obj"
	-@erase ".\outips\doc.obj"
	-@erase ".\outips\pobj.obj"
	-@erase ".\outips\oletypes.rsc"
	-@erase ".\outips\outline.rsc"
	-@erase ".\outips\server.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outips\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c 
CPP_OBJS=.\outips/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_MAC" /D\
 qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=1 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=0\
 /D qAssert=0 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 olelrf.obj ui\ole2uir.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 olelrf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="OTIS" /machine:M68K /out:"bin\outlipsd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=olelrf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OTIS" /pdb:"$(OUTDIR)/outlipsd.pdb"\
 /machine:M68K /out:"bin\outlipsd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/server.rsc"

".\bin\outlipsd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlipsd.exe"

".\bin\outlipsd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipsd.exe\
 "$(MFILE32_DEST)":outlipsd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipsd.exe\
 "$(MFILE32_DEST)":outlipsd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDebug
# PROP BASE Intermediate_Dir MacDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outld
# PROP Intermediate_Dir outld
OUTDIR=.\outld
INTDIR=.\outld

ALL : ".\bin\outld.exe" ".\bin\outld.trg"

CLEAN : 
	-@erase ".\outld\vc40.pdb"
	-@erase ".\outld\outld.pdb"
	-@erase ".\outld\except.obj"
	-@erase ".\outld\main.obj"
	-@erase ".\outld\table.obj"
	-@erase ".\outld\line.obj"
	-@erase ".\outld\toolbar.obj"
	-@erase ".\outld\debug.obj"
	-@erase ".\outld\window.obj"
	-@erase ".\outld\app.obj"
	-@erase ".\outld\util.obj"
	-@erase ".\outld\gopher.obj"
	-@erase ".\outld\pobj.obj"
	-@erase ".\outld\olexcept.obj"
	-@erase ".\outld\doc.obj"
	-@erase ".\outld\outline.rsc"
	-@erase ".\outld\oletypes.rsc"
	-@erase ".\bin\outld.trg"
	-@erase ".\bin\outld.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outld\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=0 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D DqOleCustomDragDropCursors=0_WINGS /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=0 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0\
 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D DqOleCustomDragDropCursors=0_WINGS\
 /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /Q68000 /c 
CPP_OBJS=.\outld/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=1\
 /D qAssert=1 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlmd.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 /nologo /MAC:bundle /MAC:creator="OUTL" /debug /machine:M68K /out:"bin\outld.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="OUTL"\
 /pdb:"$(OUTDIR)/outld.pdb" /debug /machine:M68K /out:"bin\outld.exe" 
LINK32_OBJS= \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/oletypes.rsc"

".\bin\outld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outld.exe"

".\bin\outld.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outld.exe "$(MFILE32_DEST)":outld.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outld.exe "$(MFILE32_DEST)":outld.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OutlineC
# PROP BASE Intermediate_Dir OutlineC
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outcd
# PROP Intermediate_Dir outcd
OUTDIR=.\outcd
INTDIR=.\outcd

ALL : ".\bin\outlcd.exe" ".\bin\outlcd.trg"

CLEAN : 
	-@erase ".\outcd\outlcd.pdb"
	-@erase ".\outcd\except.obj"
	-@erase ".\outcd\main.obj"
	-@erase ".\outcd\dxferdoc.obj"
	-@erase ".\outcd\line.obj"
	-@erase ".\outcd\appcntr.obj"
	-@erase ".\outcd\site.obj"
	-@erase ".\outcd\doc.obj"
	-@erase ".\outcd\window.obj"
	-@erase ".\outcd\olexcept.obj"
	-@erase ".\outcd\util.obj"
	-@erase ".\outcd\table.obj"
	-@erase ".\outcd\doccntr.obj"
	-@erase ".\outcd\gopher.obj"
	-@erase ".\outcd\toolbar.obj"
	-@erase ".\outcd\pobj.obj"
	-@erase ".\outcd\app.obj"
	-@erase ".\outcd\debug.obj"
	-@erase ".\outcd\oletypes.rsc"
	-@erase ".\outcd\outline.rsc"
	-@erase ".\outcd\containr.rsc"
	-@erase ".\bin\outlcd.trg"
	-@erase ".\bin\outlcd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outcd\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zd /Od /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c 
CPP_OBJS=.\outcd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1\
 /D qAssert=1 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /MAC:bundle /MAC:creator="OUTS" /pdb:"objects/outlsrd.pdb" /debug /machine:M68K /out:"apps\outlsrd.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="OUTC" /debug /machine:M68K /out:"bin\outlcd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OUTC" /pdb:"$(OUTDIR)/outlcd.pdb" /debug\
 /machine:M68K /out:"bin\outlcd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/containr.rsc"

".\bin\outlcd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlcd.exe"

".\bin\outlcd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlcd.exe "$(MFILE32_DEST)":outlcd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlcd.exe "$(MFILE32_DEST)":outlcd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OutlineS
# PROP BASE Intermediate_Dir OutlineS
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outsd
# PROP Intermediate_Dir outsd
OUTDIR=.\outsd
INTDIR=.\outsd

ALL : ".\bin\outlsrd.exe" ".\bin\outlsrd.trg"

CLEAN : 
	-@erase ".\outsd\outlsrd.pdb"
	-@erase ".\outsd\docsvr.obj"
	-@erase ".\outsd\gopher.obj"
	-@erase ".\outsd\table.obj"
	-@erase ".\outsd\util.obj"
	-@erase ".\outsd\except.obj"
	-@erase ".\outsd\olexcept.obj"
	-@erase ".\outsd\debug.obj"
	-@erase ".\outsd\appsvr.obj"
	-@erase ".\outsd\app.obj"
	-@erase ".\outsd\toolbar.obj"
	-@erase ".\outsd\pobj.obj"
	-@erase ".\outsd\main.obj"
	-@erase ".\outsd\window.obj"
	-@erase ".\outsd\dxferdoc.obj"
	-@erase ".\outsd\doc.obj"
	-@erase ".\outsd\line.obj"
	-@erase ".\outsd\outline.rsc"
	-@erase ".\outsd\server.rsc"
	-@erase ".\outsd\oletypes.rsc"
	-@erase ".\bin\outlsrd.trg"
	-@erase ".\bin\outlsrd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outsd\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D OleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c 
CPP_OBJS=.\outsd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1\
 /D qAssert=1 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlmd.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="OUTS" /debug /machine:M68K /out:"bin\outlsrd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OUTS" /pdb:"$(OUTDIR)/outlsrd.pdb" /debug\
 /machine:M68K /out:"bin\outlsrd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/server.rsc" \
	"$(INTDIR)/oletypes.rsc"

".\bin\outlsrd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlsrd.exe"

".\bin\outlsrd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlsrd.exe "$(MFILE32_DEST)":outlsrd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlsrd.exe "$(MFILE32_DEST)":outlsrd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir OutlineI
# PROP BASE Intermediate_Dir OutlineI
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outipcd
# PROP Intermediate_Dir outipcd
OUTDIR=.\outipcd
INTDIR=.\outipcd

ALL : ".\bin\outlipcd.exe" ".\bin\outlipcd.trg"

CLEAN : 
	-@erase ".\outipcd\outlipcd.pdb"
	-@erase ".\outipcd\appcntr.obj"
	-@erase ".\outipcd\dxferdoc.obj"
	-@erase ".\outipcd\line.obj"
	-@erase ".\outipcd\gopher.obj"
	-@erase ".\outipcd\site.obj"
	-@erase ".\outipcd\except.obj"
	-@erase ".\outipcd\app.obj"
	-@erase ".\outipcd\doccntr.obj"
	-@erase ".\outipcd\util.obj"
	-@erase ".\outipcd\olexcept.obj"
	-@erase ".\outipcd\toolbar.obj"
	-@erase ".\outipcd\table.obj"
	-@erase ".\outipcd\pobj.obj"
	-@erase ".\outipcd\doc.obj"
	-@erase ".\outipcd\window.obj"
	-@erase ".\outipcd\main.obj"
	-@erase ".\outipcd\debug.obj"
	-@erase ".\outipcd\oletypes.rsc"
	-@erase ".\outipcd\outline.rsc"
	-@erase ".\outipcd\containr.rsc"
	-@erase ".\bin\outlipcd.trg"
	-@erase ".\bin\outlipcd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outipcd\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Od /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D qDebug=1 /D qAssert=1 /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c 
CPP_OBJS=.\outipcd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleInPlace=1 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1\
 /D qAssert=1 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oleldf.obj ui\ole2uid.lib /nologo /MAC:bundle /MAC:creator="OUTC" /pdb:"objects/outlcd.pdb" /debug /machine:M68K /out:"apps\outlcd.exe"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="OTIC" /debug /machine:M68K /out:"bin\outlipcd.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OTIC" /pdb:"$(OUTDIR)/outlipcd.pdb" /debug\
 /machine:M68K /out:"bin\outlipcd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/containr.rsc"

".\bin\outlipcd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlipcd.exe"

".\bin\outlipcd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipcd.exe\
 "$(MFILE32_DEST)":outlipcd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipcd.exe\
 "$(MFILE32_DEST)":outlipcd.exe

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir Outline1
# PROP BASE Intermediate_Dir Outline1
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir outipsd
# PROP Intermediate_Dir outipsd
OUTDIR=.\outipsd
INTDIR=.\outipsd

ALL : ".\bin\outlipsr.exe" ".\bin\outlipsr.trg"

CLEAN : 
	-@erase ".\outipsd\vc40.pdb"
	-@erase ".\outipsd\outlipsr.pdb"
	-@erase ".\outipsd\app.obj"
	-@erase ".\outipsd\window.obj"
	-@erase ".\outipsd\util.obj"
	-@erase ".\outipsd\table.obj"
	-@erase ".\outipsd\docsvr.obj"
	-@erase ".\outipsd\toolbar.obj"
	-@erase ".\outipsd\olexcept.obj"
	-@erase ".\outipsd\gopher.obj"
	-@erase ".\outipsd\doc.obj"
	-@erase ".\outipsd\debug.obj"
	-@erase ".\outipsd\pobj.obj"
	-@erase ".\outipsd\except.obj"
	-@erase ".\outipsd\main.obj"
	-@erase ".\outipsd\line.obj"
	-@erase ".\outipsd\dxferdoc.obj"
	-@erase ".\outipsd\appsvr.obj"
	-@erase ".\outipsd\server.rsc"
	-@erase ".\outipsd\outline.rsc"
	-@erase ".\outipsd\oletypes.rsc"
	-@erase ".\bin\outlipsr.trg"
	-@erase ".\bin\outlipsr.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

.\outipsd\outl.bsc : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D USEHEADER=1 /D "_WINGS" /D "_InlineMake" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /YX /Q68000 /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /Q68000 /c 
CPP_OBJS=.\outipsd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
# ADD MRC /I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1
MRC_PROJ=/I "_WINDOWS" /I "_MAC" /I "_68K_" /D "_MSC_VER" /D "_DEBUG" /D "_MAC"\
 /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleInPlace=1 /D\
 DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D "_WINGS" /D qDebug=1\
 /D qAssert=1 /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/outl.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 oleldf.obj ui\ole2uid.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="OTIS" /debug /machine:M68K /out:"bin\outlipsr.exe"
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="OTIS" /pdb:"$(OUTDIR)/outlipsr.pdb" /debug\
 /machine:M68K /out:"bin\outlipsr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/server.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/oletypes.rsc"

".\bin\outlipsr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\bin\outlipsr.exe"

".\bin\outlipsr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipsr.exe\
 "$(MFILE32_DEST)":outlipsr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\bin\outlipsr.exe\
 "$(MFILE32_DEST)":outlipsr.exe

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

# Name "Outline - Macintosh Debug (inplace server)"
# Name "Outline - Macintosh Release"
# Name "Outline - Macintosh Release (container)"
# Name "Outline - Macintosh Release (server)"
# Name "Outline - Macintosh Release (inplace container)"
# Name "Outline - Macintosh Release (inplace server)"
# Name "Outline - Macintosh Debug"
# Name "Outline - Macintosh Debug (container)"
# Name "Outline - Macintosh Debug (server)"
# Name "Outline - Macintosh Debug (inplace container)"

!IF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\gopher.c
DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\line.c
DEP_CPP_LINE_=\
	".\Source\olehdrs.h"\
	".\Source\oledebug.h"\
	".\Source\debug.h"\
	".\Source\line.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	".\Source\const.h"\
	".\Source\doc.h"\
	".\Source\table.h"\
	".\Source\pobj.h"\
	".\Source\site.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\except.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_LINE_=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.c
DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c
DEP_CPP_POBJ_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\pobj.h"\
	".\Source\line.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\layers.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\site.h"\
	".\Source\except.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	
NODEP_CPP_POBJ_=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\app.c
DEP_CPP_APP_C=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\const.h"\
	".\Source\gopher.h"\
	".\Source\util.h"\
	".\Source\layers.h"\
	".\Source\toolbar.h"\
	".\Source\olexcept.h"\
	".\Source\dxferdoc.h"\
	".\Source\oledebug.h"\
	".\Source\line.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.c
DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\outline.r
DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=0 /D qOle=0\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.c
DEP_CPP_DOC_C=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\oledebug.h"\
	".\Source\doc.h"\
	".\Source\const.h"\
	".\Source\gopher.h"\
	".\Source\layers.h"\
	".\Source\toolbar.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	".\Source\line.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\dxferdoc.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\toolbar.c
DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\table.c
DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.c
DEP_CPP_UTIL_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	".\Source\layers.h"\
	".\Source\olexcept.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\toolbar.h"\
	".\Source\doc.h"\
	".\Source\line.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\except.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\site.h"\
	
NODEP_CPP_UTIL_=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0
# ADD CPP /D qIMacFrameTools=0 /D DqOleCustomDragDropCursors=0
# SUBTRACT CPP /D qlMacFrameTools=1 /D "qOleMessageFilter" /D = /D "0" /D qOleCustomDragDropCursors=0 /D "NDEBUG" /D qIMacFrameTools=1

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h"\
 /D "_MSC_VER" /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D\
 qOleInPlace=0 /D qOleClassFactory=0 /D qOleDragDrop=0 /D qOle=0 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qOleMessageFilter=0 /D "_WINGS"\
 /D "_InlineMake" /D qIMacFrameTools=0 /D DqOleCustomDragDropCursors=0\
 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fo"$(INTDIR)/" /Q68000 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=0 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0\
 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D DqOleCustomDragDropCursors=0_WINGS\
 /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/"\
 /Q68000 /c $(SOURCE) \
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /Q68000 /c $(SOURCE) \
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c
DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c
DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 0

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"


"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	".\Source\except.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appsvr.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

DEP_CPP_APPSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\doc.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	".\Source\appsvr.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fo"$(INTDIR)/" /Q68000 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

DEP_CPP_APPSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\doc.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	".\Source\appsvr.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 0
DEP_CPP_APPSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\doc.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	".\Source\appsvr.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

DEP_CPP_APPSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\doc.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	".\Source\appsvr.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPSV=\
	".\Source\OLine.h"\
	

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /Q68000 /c $(SOURCE) \
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\server.r

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

DEP_MRC_SERVE=\
	".\Source\const.h"\
	
NODEP_MRC_SERVE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

DEP_MRC_SERVE=\
	".\Source\const.h"\
	
NODEP_MRC_SERVE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0\
 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1 /D USEHEADER=1 /D\
 "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 0
DEP_MRC_SERVE=\
	".\Source\const.h"\
	
NODEP_MRC_SERVE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

DEP_MRC_SERVE=\
	".\Source\const.h"\
	
NODEP_MRC_SERVE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\docsvr.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

DEP_CPP_DOCSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\line.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\olexcept.h"\
	".\Source\const.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\pobj.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_DOCSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fo"$(INTDIR)/" /Q68000 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

DEP_CPP_DOCSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\line.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\olexcept.h"\
	".\Source\const.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\pobj.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_DOCSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 0
DEP_CPP_DOCSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\line.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\olexcept.h"\
	".\Source\const.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\pobj.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_DOCSV=\
	".\Source\OLine.h"\
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

DEP_CPP_DOCSV=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\line.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\olexcept.h"\
	".\Source\const.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\pobj.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_DOCSV=\
	".\Source\OLine.h"\
	

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /Q68000 /c $(SOURCE) \
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dxferdoc.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

# PROP BASE Exclude_From_Build 0
# PROP Exclude_From_Build 0
DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fo"$(INTDIR)/" /Q68000 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

# PROP Exclude_From_Build 0
DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Zd /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

DEP_CPP_DXFER=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DXFER=\
	".\Source\OLine.h"\
	

BuildCmds= \
	$(CPP) /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=1 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /Q68000 /c $(SOURCE) \
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\containr.r

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "NDEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=0 /D qAssert=0 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=0 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /I "_WINDOWS" /I "_MAC" /I "_68K_" /D\
 "_MSC_VER" /D "_DEBUG" /D "_MAC" /D qFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleInPlace=1 /D DqOleCustomDragDropCursors=1 /D qOle=1\
 /D USEHEADER=1 /D "_WINGS" /D qDebug=1 /D qAssert=1 /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\site.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	
NODEP_CPP_SITE_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	
NODEP_CPP_SITE_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	
NODEP_CPP_SITE_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	
NODEP_CPP_SITE_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appcntr.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

DEP_CPP_APPCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\const.h"\
	".\Source\olexcept.h"\
	".\Source\doc.h"\
	".\Source\gopher.h"\
	".\Source\util.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appcntr.obj" : $(SOURCE) $(DEP_CPP_APPCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_CPP_APPCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\const.h"\
	".\Source\olexcept.h"\
	".\Source\doc.h"\
	".\Source\gopher.h"\
	".\Source\util.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appcntr.obj" : $(SOURCE) $(DEP_CPP_APPCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

DEP_CPP_APPCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\const.h"\
	".\Source\olexcept.h"\
	".\Source\doc.h"\
	".\Source\gopher.h"\
	".\Source\util.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appcntr.obj" : $(SOURCE) $(DEP_CPP_APPCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_CPP_APPCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\const.h"\
	".\Source\olexcept.h"\
	".\Source\doc.h"\
	".\Source\gopher.h"\
	".\Source\util.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\site.h"\
	
NODEP_CPP_APPCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\appcntr.obj" : $(SOURCE) $(DEP_CPP_APPCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doccntr.c

!IF  "$(CFG)" == "Outline - Macintosh Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (container)"

DEP_CPP_DOCCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\line.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DOCCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doccntr.obj" : $(SOURCE) $(DEP_CPP_DOCCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D qDebug=0 /D\
 qAssert=0 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace container)"

DEP_CPP_DOCCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\line.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DOCCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doccntr.obj" : $(SOURCE) $(DEP_CPP_DOCCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=0 /D qAssert=0 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Release (inplace server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (container)"

DEP_CPP_DOCCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\line.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DOCCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doccntr.obj" : $(SOURCE) $(DEP_CPP_DOCCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 qDebug=1 /D qAssert=1 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1\
 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1\
 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D\
 "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000\
 /c $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (server)"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace container)"

DEP_CPP_DOCCN=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\line.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_DOCCN=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doccntr.obj" : $(SOURCE) $(DEP_CPP_DOCCN) "$(INTDIR)"
   $(CPP) /nologo /AL /Q68s /Q68m /GX /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_MSC_VER" /D qOleServerApp=0 /D qOle=1 /D "_DEBUG" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D qIMacFrameTools=1 /D qOleContainerApp=1 /D\
 qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D\
 qOleDragDrop=1 /D DqOleCustomDragDropCursors=1 /D "_WINGS" /D "_InlineMake" /D\
 qDebug=1 /D qAssert=1 /Fp"$(INTDIR)/outl.pch" /YX /Fo"$(INTDIR)/" /Q68000 /c\
 $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Macintosh Debug (inplace server)"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
