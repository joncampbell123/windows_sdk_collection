# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=Outline - Power Macintosh Release
!MESSAGE No configuration specified.  Defaulting to Outline - Power Macintosh\
 Release.
!ENDIF 

!IF "$(CFG)" != "Outline - Power Macintosh Release" && "$(CFG)" !=\
 "Outline - Power Macintosh Debug " && "$(CFG)" !=\
 "Outline Server - Power Macintosh Release" && "$(CFG)" !=\
 "Outline Container - Power Macintosh Release" && "$(CFG)" !=\
 "Outline InPlace Server  - Power Macintosh Release" && "$(CFG)" !=\
 "Outline InPlace Container  - Power Macintosh Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Outlppc.mak" CFG="Outline - Power Macintosh Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Outline - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Outline - Power Macintosh Debug " (based on\
 "Power Macintosh Application")
!MESSAGE "Outline Server - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Outline Container - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Outline InPlace Server  - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Outline InPlace Container  - Power Macintosh Release" (based on\
 "Power Macintosh Application")
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
# PROP Target_Last_Scanned "Outline InPlace Container  - Power Macintosh Release"
MTL=mktyplib.exe
MRC=mrc.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacRel\Outlppc.trg"
	-@erase ".\PMacRel\Outlppc.exe"
	-@erase ".\PMacRel\toolbar.obj"
	-@erase ".\PMacRel\main.obj"
	-@erase ".\PMacRel\window.obj"
	-@erase ".\PMacRel\line.obj"
	-@erase ".\PMacRel\gopher.obj"
	-@erase ".\PMacRel\app.obj"
	-@erase ".\PMacRel\util.obj"
	-@erase ".\PMacRel\oledebug.obj"
	-@erase ".\PMacRel\table.obj"
	-@erase ".\PMacRel\except.obj"
	-@erase ".\PMacRel\debug.obj"
	-@erase ".\PMacRel\pobj.obj"
	-@erase ".\PMacRel\doc.obj"
	-@erase ".\PMacRel\olexcept.obj"
	-@erase ".\PMacRel\oletypes.rsc"
	-@erase ".\PMacRel\outline.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
CPP_PROJ=/nologo /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D\
 "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D\
 qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D\
 qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D\
 qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /Fp"$(INTDIR)/Outlppc.pch"\
 /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D\
 qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OUTL" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OUTL"\
 /incremental:no /pdb:"$(OUTDIR)/Outlppc.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/oledebug.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\Outlppc.ilk"
	-@erase ".\PMacDbg\window.obj"
	-@erase ".\PMacDbg\util.obj"
	-@erase ".\PMacDbg\app.obj"
	-@erase ".\PMacDbg\toolbar.obj"
	-@erase ".\PMacDbg\gopher.obj"
	-@erase ".\PMacDbg\pobj.obj"
	-@erase ".\PMacDbg\oledebug.obj"
	-@erase ".\PMacDbg\main.obj"
	-@erase ".\PMacDbg\doc.obj"
	-@erase ".\PMacDbg\except.obj"
	-@erase ".\PMacDbg\table.obj"
	-@erase ".\PMacDbg\line.obj"
	-@erase ".\PMacDbg\debug.obj"
	-@erase ".\PMacDbg\olexcept.obj"
	-@erase ".\PMacDbg\outline.rsc"
	-@erase ".\PMacDbg\oletypes.rsc"
	-@erase ".\PMacDbg\Outlppc.pdb"
	-@erase ".\PMacDbg\Outlppc.trg"
	-@erase ".\PMacDbg\Outlppc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /GX /Zi /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=1 /D qAssert=1 /D "_WINGS" /D "_InlineMake" /c
CPP_PROJ=/nologo /GX /Zi /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D "_PPCMAC" /D pascal= /D\
 _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D\
 qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=1 /D qAssert=1 /D "_WINGS" /D\
 "_InlineMake" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D "_WINGS" /D "_InlineMake" /D qDebug=1 /D qAssert=1 /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D qOleClassFactory=0 /D\
 qOleMessageFilter=0 /D qOleDragDrop=0 /D qOleCustomDragDropCursors=0 /D qOle=0\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D "_WINGS" /D "_InlineMake" /D qDebug=1\
 /D qAssert=1 /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /incremental:no /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="OUTL" /debug /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="OUTL"\
 /incremental:yes /pdb:"$(OUTDIR)/Outlppc.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/oledebug.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/oletypes.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

!ELSEIF  "$(CFG)" == "Outline Server - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel\Server
# PROP Intermediate_Dir PMacRel\Server
OUTDIR=.\PMacRel\Server
INTDIR=.\PMacRel\Server

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacRel\Server\Outlppc.trg"
	-@erase ".\PMacRel\Server\Outlppc.exe"
	-@erase ".\PMacRel\Server\lspatch.obj"
	-@erase ".\PMacRel\Server\table.obj"
	-@erase ".\PMacRel\Server\except.obj"
	-@erase ".\PMacRel\Server\util.obj"
	-@erase ".\PMacRel\Server\appsvr.obj"
	-@erase ".\PMacRel\Server\utility.obj"
	-@erase ".\PMacRel\Server\olexcept.obj"
	-@erase ".\PMacRel\Server\debug.obj"
	-@erase ".\PMacRel\Server\app.obj"
	-@erase ".\PMacRel\Server\targtdev.obj"
	-@erase ".\PMacRel\Server\regdb.obj"
	-@erase ".\PMacRel\Server\ole2ui.obj"
	-@erase ".\PMacRel\Server\listdef.obj"
	-@erase ".\PMacRel\Server\toolbar.obj"
	-@erase ".\PMacRel\Server\pobj.obj"
	-@erase ".\PMacRel\Server\Olestd.obj"
	-@erase ".\PMacRel\Server\dbgutil.obj"
	-@erase ".\PMacRel\Server\busy.obj"
	-@erase ".\PMacRel\Server\window.obj"
	-@erase ".\PMacRel\Server\geticon.obj"
	-@erase ".\PMacRel\Server\main.obj"
	-@erase ".\PMacRel\Server\insobj.obj"
	-@erase ".\PMacRel\Server\enumfetc.obj"
	-@erase ".\PMacRel\Server\docsvr.obj"
	-@erase ".\PMacRel\Server\objfdbk.obj"
	-@erase ".\PMacRel\Server\line.obj"
	-@erase ".\PMacRel\Server\doc.obj"
	-@erase ".\PMacRel\Server\msgfiltr.obj"
	-@erase ".\PMacRel\Server\gopher.obj"
	-@erase ".\PMacRel\Server\dxferdoc.obj"
	-@erase ".\PMacRel\Server\Pastespl.obj"
	-@erase ".\PMacRel\Server\links.obj"
	-@erase ".\PMacRel\Server\convert.obj"
	-@erase ".\PMacRel\Server\common.obj"
	-@erase ".\PMacRel\Server\Ole2ui.rsc"
	-@erase ".\PMacRel\Server\outline.rsc"
	-@erase ".\PMacRel\Server\server.rsc"
	-@erase ".\PMacRel\Server\oletypes.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D\
 _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /Fp"$(INTDIR)/Outlppc.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel\Server/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="OUTS" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="OUTS" /incremental:no\
 /pdb:"$(OUTDIR)/Outlppc.pdb" /machine:MPPC /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/lspatch.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/utility.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/targtdev.obj" \
	"$(INTDIR)/regdb.obj" \
	"$(INTDIR)/ole2ui.obj" \
	"$(INTDIR)/listdef.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/Olestd.obj" \
	"$(INTDIR)/dbgutil.obj" \
	"$(INTDIR)/busy.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/geticon.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/insobj.obj" \
	"$(INTDIR)/enumfetc.obj" \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/objfdbk.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/msgfiltr.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/Pastespl.obj" \
	"$(INTDIR)/links.obj" \
	"$(INTDIR)/convert.obj" \
	"$(INTDIR)/common.obj" \
	"$(INTDIR)/Ole2ui.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/server.rsc" \
	"$(INTDIR)/oletypes.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Server\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Server\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

!ELSEIF  "$(CFG)" == "Outline Container - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel\Cntr
# PROP Intermediate_Dir PMacRel\Cntr
OUTDIR=.\PMacRel\Cntr
INTDIR=.\PMacRel\Cntr

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacRel\Cntr\Outlppc.trg"
	-@erase ".\PMacRel\Cntr\Outlppc.exe"
	-@erase ".\PMacRel\Cntr\app.obj"
	-@erase ".\PMacRel\Cntr\util.obj"
	-@erase ".\PMacRel\Cntr\debug.obj"
	-@erase ".\PMacRel\Cntr\window.obj"
	-@erase ".\PMacRel\Cntr\olexcept.obj"
	-@erase ".\PMacRel\Cntr\appcntr.obj"
	-@erase ".\PMacRel\Cntr\pobj.obj"
	-@erase ".\PMacRel\Cntr\gopher.obj"
	-@erase ".\PMacRel\Cntr\doc.obj"
	-@erase ".\PMacRel\Cntr\main.obj"
	-@erase ".\PMacRel\Cntr\except.obj"
	-@erase ".\PMacRel\Cntr\line.obj"
	-@erase ".\PMacRel\Cntr\doccntr.obj"
	-@erase ".\PMacRel\Cntr\dxferdoc.obj"
	-@erase ".\PMacRel\Cntr\table.obj"
	-@erase ".\PMacRel\Cntr\site.obj"
	-@erase ".\PMacRel\Cntr\toolbar.obj"
	-@erase ".\PMacRel\Cntr\containr.rsc"
	-@erase ".\PMacRel\Cntr\oletypes.rsc"
	-@erase ".\PMacRel\Cntr\outline.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D\
 _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /Fp"$(INTDIR)/Outlppc.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel\Cntr/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 ole2.lib /nologo /mac:bundle /mac:creator="OUTS" /machine:MPPC
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="OUTC" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="OUTC" /incremental:no\
 /pdb:"$(OUTDIR)/Outlppc.pdb" /machine:MPPC /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/containr.rsc" \
	"$(INTDIR)/oletypes.rsc" \
	"$(INTDIR)/outline.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Cntr\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\Cntr\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

!ELSEIF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel\Server
# PROP BASE Intermediate_Dir PMacRel\Server
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel\IPServer
# PROP Intermediate_Dir PMacRel\IPServer
OUTDIR=.\PMacRel\IPServer
INTDIR=.\PMacRel\IPServer

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacRel\IPServer\Outlppc.trg"
	-@erase ".\PMacRel\IPServer\Outlppc.exe"
	-@erase ".\PMacRel\IPServer\main.obj"
	-@erase ".\PMacRel\IPServer\enumfetc.obj"
	-@erase ".\PMacRel\IPServer\appsvr.obj"
	-@erase ".\PMacRel\IPServer\line.obj"
	-@erase ".\PMacRel\IPServer\utility.obj"
	-@erase ".\PMacRel\IPServer\links.obj"
	-@erase ".\PMacRel\IPServer\msgfiltr.obj"
	-@erase ".\PMacRel\IPServer\toolbar.obj"
	-@erase ".\PMacRel\IPServer\dxferdoc.obj"
	-@erase ".\PMacRel\IPServer\ole2ui.obj"
	-@erase ".\PMacRel\IPServer\listdef.obj"
	-@erase ".\PMacRel\IPServer\Pastespl.obj"
	-@erase ".\PMacRel\IPServer\window.obj"
	-@erase ".\PMacRel\IPServer\table.obj"
	-@erase ".\PMacRel\IPServer\Olestd.obj"
	-@erase ".\PMacRel\IPServer\geticon.obj"
	-@erase ".\PMacRel\IPServer\dbgutil.obj"
	-@erase ".\PMacRel\IPServer\objfdbk.obj"
	-@erase ".\PMacRel\IPServer\insobj.obj"
	-@erase ".\PMacRel\IPServer\util.obj"
	-@erase ".\PMacRel\IPServer\app.obj"
	-@erase ".\PMacRel\IPServer\docsvr.obj"
	-@erase ".\PMacRel\IPServer\debug.obj"
	-@erase ".\PMacRel\IPServer\regdb.obj"
	-@erase ".\PMacRel\IPServer\gopher.obj"
	-@erase ".\PMacRel\IPServer\convert.obj"
	-@erase ".\PMacRel\IPServer\common.obj"
	-@erase ".\PMacRel\IPServer\olexcept.obj"
	-@erase ".\PMacRel\IPServer\targtdev.obj"
	-@erase ".\PMacRel\IPServer\pobj.obj"
	-@erase ".\PMacRel\IPServer\lspatch.obj"
	-@erase ".\PMacRel\IPServer\busy.obj"
	-@erase ".\PMacRel\IPServer\except.obj"
	-@erase ".\PMacRel\IPServer\doc.obj"
	-@erase ".\PMacRel\IPServer\Ole2ui.rsc"
	-@erase ".\PMacRel\IPServer\outline.rsc"
	-@erase ".\PMacRel\IPServer\server.rsc"
	-@erase ".\PMacRel\IPServer\oletypes.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D\
 _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /Fp"$(INTDIR)/Outlppc.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel\IPServer/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=1 /D\
 qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 ole2.lib /nologo /mac:bundle /mac:creator="OUTS" /machine:MPPC
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="OTIS" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="OTIS" /incremental:no\
 /pdb:"$(OUTDIR)/Outlppc.pdb" /machine:MPPC /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/enumfetc.obj" \
	"$(INTDIR)/appsvr.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/utility.obj" \
	"$(INTDIR)/links.obj" \
	"$(INTDIR)/msgfiltr.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/ole2ui.obj" \
	"$(INTDIR)/listdef.obj" \
	"$(INTDIR)/Pastespl.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/Olestd.obj" \
	"$(INTDIR)/geticon.obj" \
	"$(INTDIR)/dbgutil.obj" \
	"$(INTDIR)/objfdbk.obj" \
	"$(INTDIR)/insobj.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/docsvr.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/regdb.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/convert.obj" \
	"$(INTDIR)/common.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/targtdev.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/lspatch.obj" \
	"$(INTDIR)/busy.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/Ole2ui.rsc" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/server.rsc" \
	"$(INTDIR)/oletypes.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\IPServer\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\IPServer\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

!ELSEIF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel\IPCntr
# PROP Intermediate_Dir PMacRel\IPCntr
OUTDIR=.\PMacRel\IPCntr
INTDIR=.\PMacRel\IPCntr

ALL : "$(OUTDIR)\Outlppc.exe" "$(OUTDIR)\Outlppc.trg"

CLEAN : 
	-@erase ".\PMacRel\IPCntr\Outlppc.trg"
	-@erase ".\PMacRel\IPCntr\Outlppc.exe"
	-@erase ".\PMacRel\IPCntr\doccntr.obj"
	-@erase ".\PMacRel\IPCntr\common.obj"
	-@erase ".\PMacRel\IPCntr\table.obj"
	-@erase ".\PMacRel\IPCntr\except.obj"
	-@erase ".\PMacRel\IPCntr\utility.obj"
	-@erase ".\PMacRel\IPCntr\olexcept.obj"
	-@erase ".\PMacRel\IPCntr\line.obj"
	-@erase ".\PMacRel\IPCntr\targtdev.obj"
	-@erase ".\PMacRel\IPCntr\toolbar.obj"
	-@erase ".\PMacRel\IPCntr\site.obj"
	-@erase ".\PMacRel\IPCntr\listdef.obj"
	-@erase ".\PMacRel\IPCntr\debug.obj"
	-@erase ".\PMacRel\IPCntr\app.obj"
	-@erase ".\PMacRel\IPCntr\regdb.obj"
	-@erase ".\PMacRel\IPCntr\dbgutil.obj"
	-@erase ".\PMacRel\IPCntr\geticon.obj"
	-@erase ".\PMacRel\IPCntr\enumfetc.obj"
	-@erase ".\PMacRel\IPCntr\ole2ui.obj"
	-@erase ".\PMacRel\IPCntr\objfdbk.obj"
	-@erase ".\PMacRel\IPCntr\util.obj"
	-@erase ".\PMacRel\IPCntr\appcntr.obj"
	-@erase ".\PMacRel\IPCntr\Olestd.obj"
	-@erase ".\PMacRel\IPCntr\msgfiltr.obj"
	-@erase ".\PMacRel\IPCntr\window.obj"
	-@erase ".\PMacRel\IPCntr\dxferdoc.obj"
	-@erase ".\PMacRel\IPCntr\Pastespl.obj"
	-@erase ".\PMacRel\IPCntr\insobj.obj"
	-@erase ".\PMacRel\IPCntr\convert.obj"
	-@erase ".\PMacRel\IPCntr\doc.obj"
	-@erase ".\PMacRel\IPCntr\pobj.obj"
	-@erase ".\PMacRel\IPCntr\lspatch.obj"
	-@erase ".\PMacRel\IPCntr\links.obj"
	-@erase ".\PMacRel\IPCntr\busy.obj"
	-@erase ".\PMacRel\IPCntr\gopher.obj"
	-@erase ".\PMacRel\IPCntr\main.obj"
	-@erase ".\PMacRel\IPCntr\outline.rsc"
	-@erase ".\PMacRel\IPCntr\Ole2ui.rsc"
	-@erase ".\PMacRel\IPCntr\containr.rsc"
	-@erase ".\PMacRel\IPCntr\oletypes.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/OutlPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D\
 _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /Fp"$(INTDIR)/Outlppc.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel\IPCntr/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
# ADD MRC /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /NOLOGO
MRC_PROJ=/D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "NDEBUG" /D "_PPCMAC" /D\
 pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1 /D qOleServerApp=0 /D\
 qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D qOleClassFactory=1 /D\
 qOleMessageFilter=1 /D qOleDragDrop=1 /D qOleCustomDragDropCursors=1 /D qOle=1\
 /D qIMacFrameTools=1 /D qFrameTools=1 /D qDebug=0 /D qAssert=0 /D "_WINGS" /D\
 "_InlineMake" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Outlppc.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 ole2.lib /nologo /mac:bundle /mac:creator="OUTC" /machine:MPPC
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="OTIC" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="OTIC" /incremental:no\
 /pdb:"$(OUTDIR)/Outlppc.pdb" /machine:MPPC /out:"$(OUTDIR)/Outlppc.exe" 
LINK32_OBJS= \
	"$(INTDIR)/doccntr.obj" \
	"$(INTDIR)/common.obj" \
	"$(INTDIR)/table.obj" \
	"$(INTDIR)/except.obj" \
	"$(INTDIR)/utility.obj" \
	"$(INTDIR)/olexcept.obj" \
	"$(INTDIR)/line.obj" \
	"$(INTDIR)/targtdev.obj" \
	"$(INTDIR)/toolbar.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/listdef.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/app.obj" \
	"$(INTDIR)/regdb.obj" \
	"$(INTDIR)/dbgutil.obj" \
	"$(INTDIR)/geticon.obj" \
	"$(INTDIR)/enumfetc.obj" \
	"$(INTDIR)/ole2ui.obj" \
	"$(INTDIR)/objfdbk.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/appcntr.obj" \
	"$(INTDIR)/Olestd.obj" \
	"$(INTDIR)/msgfiltr.obj" \
	"$(INTDIR)/window.obj" \
	"$(INTDIR)/dxferdoc.obj" \
	"$(INTDIR)/Pastespl.obj" \
	"$(INTDIR)/insobj.obj" \
	"$(INTDIR)/convert.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/pobj.obj" \
	"$(INTDIR)/lspatch.obj" \
	"$(INTDIR)/links.obj" \
	"$(INTDIR)/busy.obj" \
	"$(INTDIR)/gopher.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/outline.rsc" \
	"$(INTDIR)/Ole2ui.rsc" \
	"$(INTDIR)/containr.rsc" \
	"$(INTDIR)/oletypes.rsc"

"$(OUTDIR)\Outlppc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Outlppc.exe"

"$(OUTDIR)\Outlppc.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\IPCntr\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\IPCntr\Outlppc.exe\
 "$(MFILE32_DEST):Outlppc.exe">"$(OUTDIR)\Outlppc.trg"

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

# Name "Outline - Power Macintosh Release"
# Name "Outline - Power Macintosh Debug "

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\app.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\debug.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

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

SOURCE=.\Source\doc.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

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

SOURCE=.\Source\gopher.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

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

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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

SOURCE=.\Source\main.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline - Power Macintosh Release"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D\
 qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D\
 qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "_DEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D\
 qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D\
 qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 "_WINGS" /D "_InlineMake" /D qDebug=1 /D qAssert=1 /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\outline.r

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D\
 qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D\
 qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "_DEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=0 /D qOleTreatAs=0 /D qOleInPlace=0 /D\
 qOleClassFactory=0 /D qOleMessageFilter=0 /D qOleDragDrop=0 /D\
 qOleCustomDragDropCursors=0 /D qOle=0 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 "_WINGS" /D "_InlineMake" /D qDebug=1 /D qAssert=1 /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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

SOURCE=.\Source\table.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\toolbar.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\util.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

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

SOURCE=.\Source\oledebug.c

!IF  "$(CFG)" == "Outline - Power Macintosh Release"

DEP_CPP_OLEDE=\
	".\Source\olehdrs.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_OLEDE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\oledebug.obj" : $(SOURCE) $(DEP_CPP_OLEDE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Outline - Power Macintosh Debug "

DEP_CPP_OLEDE=\
	".\Source\olehdrs.h"\
	".\Source\oledebug.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_OLEDE=\
	".\Source\OLine.h"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\oledebug.obj" : $(SOURCE) $(DEP_CPP_OLEDE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEDE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Outline Server - Power Macintosh Release"
################################################################################
# Begin Source File

SOURCE=.\Source\app.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\gopher.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\line.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\outline.r

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\table.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\toolbar.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\busy.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_BUSY_=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\busy.h"\
	
NODEP_CPP_BUSY_=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\busy.obj" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\common.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_COMMO=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_COMMO=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\convert.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_CONVE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\convert.h"\
	
NODEP_CPP_CONVE=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\dbgutil.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"


"$(INTDIR)\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\enumfetc.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_ENUMF=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_ENUMF=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\enumfetc.obj" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\geticon.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

NODEP_CPP_GETIC=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\insobj.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_INSOB=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\insobj.h"\
	
NODEP_CPP_INSOB=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\insobj.obj" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\links.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_LINKS=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_LINKS=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\listdef.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_LISTD=\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\common.h"\
	

"$(INTDIR)\listdef.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\lspatch.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"


"$(INTDIR)\lspatch.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\msgfiltr.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_MSGFI=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_MSGFI=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\msgfiltr.obj" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\objfdbk.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_OBJFD=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OBJFD=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\objfdbk.obj" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\ole2ui.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_OLE2U=\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLE2U=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\ole2ui.obj" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Olestd.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_OLEST=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLEST=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\Olestd.obj" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Pastespl.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_PASTE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\pastespl.h"\
	
NODEP_CPP_PASTE=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\Pastespl.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\regdb.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_REGDB=\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_REGDB=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\regdb.obj" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\targtdev.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_TARGT=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_TARGT=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\targtdev.obj" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\utility.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_UTILI=\
	".\..\UILib\Source\olestd.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_UTILI=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Ole2ui.r

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"


"$(INTDIR)\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dxferdoc.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_DXFER=\
	".\Source\OLine.h"\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oledebug.h"\
	".\Source\dxferdoc.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\docsvr.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_DOCSV=\
	".\Source\OLine.h"\
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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appsvr.c

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_CPP_APPSV=\
	".\Source\OLine.h"\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\pobj.h"\
	".\Source\doc.h"\
	".\Source\olexcept.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	".\Source\appsvr.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\server.r

!IF  "$(CFG)" == "Outline Server - Power Macintosh Release"

DEP_MRC_SERVE=\
	".\Source\OLine.h"\
	".\Source\const.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Outline Container - Power Macintosh Release"
################################################################################
# Begin Source File

SOURCE=.\Source\app.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\gopher.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\line.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\table.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\toolbar.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dxferdoc.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appcntr.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doccntr.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\site.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\containr.r

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\outline.r

!IF  "$(CFG)" == "Outline Container - Power Macintosh Release"

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=0 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Outline InPlace Server  - Power Macintosh Release"
################################################################################
# Begin Source File

SOURCE=.\Source\app.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\gopher.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\line.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\outline.r

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\table.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\toolbar.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_WINDO=\
	".\Source\OLine.h"\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\busy.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\busy.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\common.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\common.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\convert.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\convert.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\dbgutil.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\enumfetc.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\enumfetc.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\geticon.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\insobj.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\insobj.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\links.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\links.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\listdef.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_CPP_LISTD=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\links.h"\
	

"$(INTDIR)\listdef.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\lspatch.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\lspatch.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\msgfiltr.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\msgfiltr.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\objfdbk.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\objfdbk.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\ole2ui.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\ole2ui.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Olestd.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\Olestd.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Pastespl.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\Pastespl.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\regdb.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\regdb.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\targtdev.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\targtdev.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\utility.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\utility.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Ole2ui.r

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"


"$(INTDIR)\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dxferdoc.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\OLine.h"\
	

"$(INTDIR)\dxferdoc.obj" : $(SOURCE) $(DEP_CPP_DXFER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\docsvr.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\site.h"\
	".\Source\window.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\OLine.h"\
	

"$(INTDIR)\docsvr.obj" : $(SOURCE) $(DEP_CPP_DOCSV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appsvr.c

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\line.h"\
	".\Source\dxferdoc.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\OLine.h"\
	

"$(INTDIR)\appsvr.obj" : $(SOURCE) $(DEP_CPP_APPSV) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\server.r

!IF  "$(CFG)" == "Outline InPlace Server  - Power Macintosh Release"

DEP_MRC_SERVE=\
	".\Source\const.h"\
	".\Source\OLine.h"\
	

"$(INTDIR)\server.rsc" : $(SOURCE) $(DEP_MRC_SERVE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/server.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=1 /D qOleContainerApp=0 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Outline InPlace Container  - Power Macintosh Release"
################################################################################
# Begin Source File

SOURCE=.\Source\app.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	
NODEP_CPP_APP_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\app.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_DEBUG=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_DEBUG=\
	".\Source\OLine.h"\
	".\Source\DebugWindow.h"\
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\docsvr.h"\
	".\Source\doccntr.h"\
	".\Source\except.h"\
	".\Source\site.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_DOC_C=\
	".\Source\OLine.h"\
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\gopher.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_GOPHE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\gopher.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_GOPHE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\gopher.obj" : $(SOURCE) $(DEP_CPP_GOPHE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\line.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\line.obj" : $(SOURCE) $(DEP_CPP_LINE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_MAIN_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\const.h"\
	".\Source\app.h"\
	".\Source\olexcept.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\OLine.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olexcept.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_OLEXC=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	
NODEP_CPP_OLEXC=\
	".\Source\OLine.h"\
	

"$(INTDIR)\olexcept.obj" : $(SOURCE) $(DEP_CPP_OLEXC) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pobj.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\pobj.obj" : $(SOURCE) $(DEP_CPP_POBJ_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\table.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_TABLE=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\table.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	
NODEP_CPP_TABLE=\
	".\Source\OLine.h"\
	

"$(INTDIR)\table.obj" : $(SOURCE) $(DEP_CPP_TABLE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\toolbar.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_TOOLB=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\toolbar.h"\
	".\Source\const.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\window.h"\
	".\Source\except.h"\
	".\Source\app.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	
NODEP_CPP_TOOLB=\
	".\Source\OLine.h"\
	

"$(INTDIR)\toolbar.obj" : $(SOURCE) $(DEP_CPP_TOOLB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\window.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_WINDO=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\window.h"\
	".\Source\layers.h"\
	".\Source\util.h"\
	".\Source\olexcept.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\const.h"\
	".\Source\appsvr.h"\
	".\Source\appcntr.h"\
	".\Source\except.h"\
	
NODEP_CPP_WINDO=\
	".\Source\OLine.h"\
	

"$(INTDIR)\window.obj" : $(SOURCE) $(DEP_CPP_WINDO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\busy.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_BUSY_=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\busy.h"\
	
NODEP_CPP_BUSY_=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\busy.obj" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\common.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_COMMO=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_COMMO=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\convert.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_CONVE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\convert.h"\
	
NODEP_CPP_CONVE=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\enumfetc.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_ENUMF=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_ENUMF=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\enumfetc.obj" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\geticon.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

NODEP_CPP_GETIC=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\insobj.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_INSOB=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\insobj.h"\
	
NODEP_CPP_INSOB=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\insobj.obj" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\links.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_LINKS=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_LINKS=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\listdef.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_LISTD=\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\common.h"\
	

"$(INTDIR)\listdef.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\lspatch.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"


"$(INTDIR)\lspatch.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\msgfiltr.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_MSGFI=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_MSGFI=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\msgfiltr.obj" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\objfdbk.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_OBJFD=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OBJFD=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\objfdbk.obj" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\ole2ui.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_OLE2U=\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLE2U=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\ole2ui.obj" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Olestd.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_OLEST=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLEST=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\Olestd.obj" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Pastespl.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_PASTE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\pastespl.h"\
	
NODEP_CPP_PASTE=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\Pastespl.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\targtdev.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_TARGT=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_TARGT=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\targtdev.obj" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\utility.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_UTILI=\
	".\..\UILib\Source\olestd.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_UTILI=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\Ole2ui.r

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"


"$(INTDIR)\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dxferdoc.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\appcntr.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doccntr.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

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
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\site.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_SITE_=\
	".\Source\olehdrs.h"\
	".\Source\debug.h"\
	".\Source\app.h"\
	".\Source\doc.h"\
	".\Source\site.h"\
	".\Source\oledebug.h"\
	".\Source\util.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
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
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\containr.r

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_MRC_CONTA=\
	".\Source\const.h"\
	
NODEP_MRC_CONTA=\
	".\Source\OLine.h"\
	

"$(INTDIR)\containr.rsc" : $(SOURCE) $(DEP_MRC_CONTA) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/containr.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\dbgutil.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"


"$(INTDIR)\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\except.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_EXCEP=\
	".\Source\olehdrs.h"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	".\Source\except.h"\
	

"$(INTDIR)\except.obj" : $(SOURCE) $(DEP_CPP_EXCEP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\oletypes.r

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"


"$(INTDIR)\oletypes.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/oletypes.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\outline.r

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_MRC_OUTLI=\
	".\Source\const.h"\
	
NODEP_MRC_OUTLI=\
	".\Source\OLine.h"\
	

"$(INTDIR)\outline.rsc" : $(SOURCE) $(DEP_MRC_OUTLI) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/outline.rsc" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D\
 "NDEBUG" /D "_PPCMAC" /D pascal= /D _pascal= /D __pascal= /D qlMacFrameTools=1\
 /D qOleServerApp=0 /D qOleContainerApp=1 /D qOleTreatAs=1 /D qOleInPlace=1 /D\
 qOleClassFactory=1 /D qOleMessageFilter=1 /D qOleDragDrop=1 /D\
 qOleCustomDragDropCursors=1 /D qOle=1 /D qIMacFrameTools=1 /D qFrameTools=1 /D\
 qDebug=0 /D qAssert=0 /D "_WINGS" /D "_InlineMake" /l 0x409 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\UILib\Source\regdb.c

!IF  "$(CFG)" == "Outline InPlace Container  - Power Macintosh Release"

DEP_CPP_REGDB=\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_REGDB=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\regdb.obj" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
