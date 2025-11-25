# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=simpsrvr - Macintosh SimpleServer Debug
!MESSAGE No configuration specified.  Defaulting to simpsrvr - Macintosh\
 SimpleServer Debug.
!ENDIF 

!IF "$(CFG)" != "simpsrvr - Macintosh SimpleServer Release" && "$(CFG)" !=\
 "simpsrvr - Macintosh SimpleServer Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "simpsrvr.mak" CFG="simpsrvr - Macintosh SimpleServer Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "simpsrvr - Macintosh SimpleServer Release" (based on "Macintosh Application")
!MESSAGE "simpsrvr - Macintosh SimpleServer Debug" (based on\
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
# PROP Target_Last_Scanned "simpsrvr - Macintosh SimpleServer Debug"
MTL=mktyplib.exe
RSC=rc.exe
MRC=mrc.exe
CPP=cl.exe

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir simpr
# PROP Intermediate_Dir simpr
OUTDIR=.\simpr
INTDIR=.\simpr

ALL : ".\simpr\simpsrvr.exe" ".\simpr\simpsrvr.trg"

CLEAN : 
	-@erase ".\simpr\simpsrvr.trg"
	-@erase ".\simpr\simpsrvr.exe"
	-@erase ".\simpr\main.obj"
	-@erase ".\simpr\doc.obj"
	-@erase ".\simpr\icf.obj"
	-@erase ".\simpr\Ips.obj"
	-@erase ".\simpr\cobject.obj"
	-@erase ".\simpr\debug.obj"
	-@erase ".\simpr\Aevents.obj"
	-@erase ".\simpr\util.obj"
	-@erase ".\simpr\imf.obj"
	-@erase ".\simpr\App.obj"
	-@erase ".\simpr\ctoolbar.obj"
	-@erase ".\simpr\ido.obj"
	-@erase ".\simpr\oleerror.obj"
	-@erase ".\simpr\cshape.obj"
	-@erase ".\simpr\ioo.obj"
	-@erase ".\simpr\Cwindow.obj"
	-@erase ".\simpr\simpsvr.rsc"
	-@erase ".\simpr\Ole2ui.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /I "_MAC_" /FI"macole\msvcole.h" /D "NDEBUG" /D "_MSC_VER" /D "_MAC" /D "_68K_" /YX /c
CPP_PROJ=/nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /I "_MAC_"\
 /FI"macole\msvcole.h" /D "NDEBUG" /D "_MSC_VER" /D "_MAC" /D "_68K_"\
 /Fp"$(INTDIR)/simpsrvr.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\simpr/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /I "..\uilib\source" /D "NDEBUG" /D "_MSC_VER" /D "_MAC"
MRC_PROJ=/I "..\uilib\source" /D "NDEBUG" /D "_MSC_VER" /D "_MAC" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/simpsrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="SiSe" /machine:M68K
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="SiSe" /pdb:"$(OUTDIR)/simpsrvr.pdb"\
 /machine:M68K /out:"$(OUTDIR)/simpsrvr.exe" 
LINK32_OBJS= \
	".\simpr\main.obj" \
	".\simpr\doc.obj" \
	".\simpr\icf.obj" \
	".\simpr\Ips.obj" \
	".\simpr\cobject.obj" \
	".\simpr\debug.obj" \
	".\simpr\Aevents.obj" \
	".\simpr\util.obj" \
	".\simpr\imf.obj" \
	".\simpr\App.obj" \
	".\simpr\ctoolbar.obj" \
	".\simpr\ido.obj" \
	".\simpr\oleerror.obj" \
	".\simpr\cshape.obj" \
	".\simpr\ioo.obj" \
	".\simpr\Cwindow.obj" \
	".\simpr\simpsvr.rsc" \
	".\simpr\Ole2ui.rsc"

".\simpr\simpsrvr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\simpr\simpsrvr.exe"

".\simpr\simpsrvr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpr\simpsrvr.exe\
 "$(MFILE32_DEST)":simpsrvr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpr\simpsrvr.exe\
 "$(MFILE32_DEST)":simpsrvr.exe

!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDebug
# PROP BASE Intermediate_Dir MacDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir simpd
# PROP Intermediate_Dir simpd
OUTDIR=.\simpd
INTDIR=.\simpd

ALL : ".\simpd\simpsrvd.exe" ".\simpd\simpsrvd.trg"

CLEAN : 
	-@erase ".\simpd\vc40.pdb"
	-@erase ".\simpd\simpsrvd.pdb"
	-@erase ".\simpd\ctoolbar.obj"
	-@erase ".\simpd\oleerror.obj"
	-@erase ".\simpd\Cwindow.obj"
	-@erase ".\simpd\imf.obj"
	-@erase ".\simpd\util.obj"
	-@erase ".\simpd\App.obj"
	-@erase ".\simpd\ido.obj"
	-@erase ".\simpd\cobject.obj"
	-@erase ".\simpd\Aevents.obj"
	-@erase ".\simpd\ioo.obj"
	-@erase ".\simpd\cshape.obj"
	-@erase ".\simpd\debug.obj"
	-@erase ".\simpd\doc.obj"
	-@erase ".\simpd\main.obj"
	-@erase ".\simpd\icf.obj"
	-@erase ".\simpd\Ips.obj"
	-@erase ".\simpd\simpsvr.rsc"
	-@erase ".\simpd\Ole2ui.rsc"
	-@erase ".\simpd\simpsrvd.trg"
	-@erase ".\simpd\simpsrvd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source" /I "_MAC_" /FI"macole\msvcole.h" /D "_DEBUG" /D DEBUG=1 /D "_MSC_VER" /D "_MAC" /D "_68K_" /YX /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source" /I "_MAC_"\
 /FI"macole\msvcole.h" /D "_DEBUG" /D DEBUG=1 /D "_MSC_VER" /D "_MAC" /D "_68K_"\
 /Fp"$(INTDIR)/simpsrvr.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\simpd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /I "..\uilib\source" /D "_DEBUG" /D "DEBUG" /D "_MSC_VER" /D "_MAC"
MRC_PROJ=/I "..\uilib\source" /D "_DEBUG" /D "DEBUG" /D "_MSC_VER" /D "_MAC" /l\
 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/simpsrvr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlmd.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="SiSe" /debug /machine:M68K /out:"simpd/simpsrvd.exe"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="SiSe" /pdb:"$(OUTDIR)/simpsrvd.pdb" /debug\
 /machine:M68K /out:"$(OUTDIR)/simpsrvd.exe" 
LINK32_OBJS= \
	".\simpd\ctoolbar.obj" \
	".\simpd\oleerror.obj" \
	".\simpd\Cwindow.obj" \
	".\simpd\imf.obj" \
	".\simpd\util.obj" \
	".\simpd\App.obj" \
	".\simpd\ido.obj" \
	".\simpd\cobject.obj" \
	".\simpd\Aevents.obj" \
	".\simpd\ioo.obj" \
	".\simpd\cshape.obj" \
	".\simpd\debug.obj" \
	".\simpd\doc.obj" \
	".\simpd\main.obj" \
	".\simpd\icf.obj" \
	".\simpd\Ips.obj" \
	".\simpd\simpsvr.rsc" \
	".\simpd\Ole2ui.rsc"

".\simpd\simpsrvd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\simpd\simpsrvd.exe"

".\simpd\simpsrvd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpd\simpsrvd.exe\
 "$(MFILE32_DEST)":simpsrvd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpd\simpsrvd.exe\
 "$(MFILE32_DEST)":simpsrvd.exe

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

# Name "simpsrvr - Macintosh SimpleServer Release"
# Name "simpsrvr - Macintosh SimpleServer Debug"

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"

!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\simpsvr.r

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\simpsvr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpsvr.rsc" /I "..\uilib\source" /D "NDEBUG" /D\
 "_MSC_VER" /D "_MAC" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


".\simpd\simpsvr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpsvr.rsc" /I "..\uilib\source" /D "_DEBUG" /D "DEBUG"\
 /D "_MSC_VER" /D "_MAC" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Aevents.cpp
DEP_CPP_AEVEN=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\Aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\Aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Ips.cpp
DEP_CPP_IPS_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\Ips.obj" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\Ips.obj" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\doc.cpp
DEP_CPP_DOC_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.cpp
DEP_CPP_DEBUG=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ido.cpp
DEP_CPP_IDO_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\ido.obj" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\ido.obj" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\imf.cpp
DEP_CPP_IMF_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Cwindow.cpp
DEP_CPP_CWIND=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\Cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\Cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\cobject.cpp
DEP_CPP_COBJE=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\cobject.obj" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\cobject.obj" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ctoolbar.cpp
DEP_CPP_CTOOL=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\ctoolbar.obj" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\ctoolbar.obj" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ioo.cpp
DEP_CPP_IOO_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\ioo.obj" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\ioo.obj" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\icf.cpp
DEP_CPP_ICF_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\icf.obj" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\icf.obj" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\App.cpp
DEP_CPP_APP_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\cshape.cpp
DEP_CPP_CSHAP=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\cshape.obj" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\cshape.obj" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\oleerror.cpp
DEP_CPP_OLEER=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\oleerror.obj" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\oleerror.obj" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.cpp
DEP_CPP_MAIN_=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\util.cpp
DEP_CPP_UTIL_=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cobject.h"\
	".\Source\cshape.h"\
	".\Source\ctoolbar.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\doc.h"\
	".\Source\oleerror.h"\
	".\Source\scodes.h"\
	".\Source\util.h"\
	".\Source\icf.h"\
	".\Source\ido.h"\
	".\Source\imf.h"\
	".\Source\ioo.h"\
	".\Source\ips.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\simpd\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

".\simpd\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Ole2ui.r

!IF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Release"


".\simpr\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /I "..\uilib\source" /D "NDEBUG" /D\
 "_MSC_VER" /D "_MAC" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "simpsrvr - Macintosh SimpleServer Debug"


".\simpd\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /I "..\uilib\source" /D "_DEBUG" /D "DEBUG"\
 /D "_MSC_VER" /D "_MAC" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
