# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Simpcntr - Macintosh SimpleContainer Debug
!MESSAGE No configuration specified.  Defaulting to Simpcntr - Macintosh\
 SimpleContainer Debug.
!ENDIF 

!IF "$(CFG)" != "Simpcntr - Macintosh SimpleContainer Release" && "$(CFG)" !=\
 "Simpcntr - Macintosh SimpleContainer Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Simpcntr.mak"\
 CFG="Simpcntr - Macintosh SimpleContainer Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Simpcntr - Macintosh SimpleContainer Release" (based on\
 "Macintosh Application")
!MESSAGE "Simpcntr - Macintosh SimpleContainer Debug" (based on\
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
# PROP Target_Last_Scanned "Simpcntr - Macintosh SimpleContainer Debug"
MRC=mrc.exe
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"

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

ALL : "$(OUTDIR)\Simpcntr.exe" "$(OUTDIR)\Simpcntr.trg"

CLEAN : 
	-@erase ".\simpr\Simpcntr.trg"
	-@erase ".\simpr\Simpcntr.exe"
	-@erase ".\simpr\util.obj"
	-@erase ".\simpr\cerrors.obj"
	-@erase ".\simpr\aevents.obj"
	-@erase ".\simpr\iocs.obj"
	-@erase ".\simpr\Imf.obj"
	-@erase ".\simpr\App.obj"
	-@erase ".\simpr\main.obj"
	-@erase ".\simpr\debug.obj"
	-@erase ".\simpr\cwindow.obj"
	-@erase ".\simpr\ias.obj"
	-@erase ".\simpr\site.obj"
	-@erase ".\simpr\simpcntr.rsc"
	-@erase ".\simpr\Ole2ui.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h" /D "NDEBUG" /D "_MAC" /D "_68K_" /D "_MSC_VER" /YX /c
CPP_PROJ=/nologo /AL /Q68s /GX /O2 /I "..\uilib\source" /FI"macole\msvcole.h"\
 /D "NDEBUG" /D "_MAC" /D "_68K_" /D "_MSC_VER" /Fp"$(INTDIR)/Simpcntr.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\simpr/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /I "..\uilib\source" /D "NDEBUG" /D "_MAC" /D "_MSC_VER"
MRC_PROJ=/I "..\uilib\source" /D "NDEBUG" /D "_MAC" /D "_MSC_VER" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Simpcntr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle /MAC:creator="SiCo" /pdb:none /machine:M68K
LINK32_FLAGS=oleldf.obj ..\uilib\uilibr\ole2uir.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="SiCo" /pdb:none /machine:M68K\
 /out:"$(OUTDIR)/Simpcntr.exe" 
LINK32_OBJS= \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/cerrors.obj" \
	"$(INTDIR)/aevents.obj" \
	"$(INTDIR)/iocs.obj" \
	"$(INTDIR)/Imf.obj" \
	"$(INTDIR)/App.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/cwindow.obj" \
	"$(INTDIR)/ias.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/simpcntr.rsc" \
	"$(INTDIR)/Ole2ui.rsc"

"$(OUTDIR)\Simpcntr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/Simpcntr.exe"

"$(OUTDIR)\Simpcntr.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpr\Simpcntr.exe\
 "$(MFILE32_DEST)":Simpcntr.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpr\Simpcntr.exe\
 "$(MFILE32_DEST)":Simpcntr.exe

!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"

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

ALL : "$(OUTDIR)\simpcntd.exe" "$(OUTDIR)\simpcntd.trg"

CLEAN : 
	-@erase ".\simpd\vc40.pdb"
	-@erase ".\simpd\simpcntd.trg"
	-@erase ".\simpd\simpcntd.exe"
	-@erase ".\simpd\cwindow.obj"
	-@erase ".\simpd\iocs.obj"
	-@erase ".\simpd\debug.obj"
	-@erase ".\simpd\Imf.obj"
	-@erase ".\simpd\App.obj"
	-@erase ".\simpd\main.obj"
	-@erase ".\simpd\cerrors.obj"
	-@erase ".\simpd\aevents.obj"
	-@erase ".\simpd\ias.obj"
	-@erase ".\simpd\site.obj"
	-@erase ".\simpd\util.obj"
	-@erase ".\simpd\Ole2ui.rsc"
	-@erase ".\simpd\simpcntr.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source" /FI"macole\msvcole.h" /D "_DEBUG" /D "_MAC" /D "_68K_" /D "_MSC_VER" /D DEBUG=1 /YX /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /GX /Zi /Od /I "..\uilib\source"\
 /FI"macole\msvcole.h" /D "_DEBUG" /D "_MAC" /D "_68K_" /D "_MSC_VER" /D DEBUG=1\
 /Fp"$(INTDIR)/Simpcntr.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\simpd/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /I "..\uilib\source" /D "_DEBUG" /D "_MAC" /D "_MSC_VER" /D "DEBUG"
MRC_PROJ=/I "..\uilib\source" /D "_DEBUG" /D "_MAC" /D "_MSC_VER" /D "DEBUG" /l\
 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Simpcntr.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc wlmd.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle /MAC:creator="SiCo" /pdb:none /debug /machine:M68K /out:"simpd/simpcntd.exe"
LINK32_FLAGS=oleldf.obj ..\uilib\uilibd\ole2uid.lib /nologo /MAC:bundle\
 /MAC:type="APPL" /MAC:creator="SiCo" /pdb:none /debug /machine:M68K\
 /out:"$(OUTDIR)/simpcntd.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cwindow.obj" \
	"$(INTDIR)/iocs.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/Imf.obj" \
	"$(INTDIR)/App.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/cerrors.obj" \
	"$(INTDIR)/aevents.obj" \
	"$(INTDIR)/ias.obj" \
	"$(INTDIR)/site.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/Ole2ui.rsc" \
	"$(INTDIR)/simpcntr.rsc"

"$(OUTDIR)\simpcntd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/simpcntd.exe"

"$(OUTDIR)\simpcntd.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpd\simpcntd.exe\
 "$(MFILE32_DEST)":simpcntd.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\simpd\simpcntd.exe\
 "$(MFILE32_DEST)":simpcntd.exe

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

# Name "Simpcntr - Macintosh SimpleContainer Release"
# Name "Simpcntr - Macintosh SimpleContainer Debug"

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"

!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\site.cpp
DEP_CPP_SITE_=\
	".\Source\Hdrs.h"\
	".\Source\mac.h"\
	".\Source\aevents.h"\
	".\Source\util.h"\
	".\Source\cerrors.h"\
	".\Source\app.h"\
	".\Source\ias.h"\
	".\Source\site.h"\
	".\Source\iocs.h"\
	".\Source\cwindow.h"\
	".\Source\debug.h"\
	".\Source\Mycoguid.h"\
	".\Source\imf.h"\
	".\Source\scodes.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	
NODEP_CPP_SITE_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
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
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_DEBUG=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


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

SOURCE=.\Source\iocs.cpp
DEP_CPP_IOCS_=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IOCS_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\iocs.obj" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\iocs.obj" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ias.cpp
DEP_CPP_IAS_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IAS_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\ias.obj" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ias.obj" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\aevents.cpp
DEP_CPP_AEVEN=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_AEVEN=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Imf.cpp
DEP_CPP_IMF_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IMF_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\Imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\cerrors.cpp
DEP_CPP_CERRO=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_CERRO=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\cerrors.obj" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cerrors.obj" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\cwindow.cpp
DEP_CPP_CWIND=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_CWIND=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\simpcntr.r

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\simpcntr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpcntr.rsc" /I "..\uilib\source" /D "NDEBUG" /D "_MAC"\
 /D "_MSC_VER" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


"$(INTDIR)\simpcntr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpcntr.rsc" /I "..\uilib\source" /D "_DEBUG" /D "_MAC"\
 /D "_MSC_VER" /D "DEBUG" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\App.cpp
DEP_CPP_APP_C=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_APP_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
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
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


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

SOURCE=.\Source\util.cpp
DEP_CPP_UTIL_=\
	".\Source\Hdrs.h"\
	".\Source\aevents.h"\
	".\Source\app.h"\
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_UTIL_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


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

SOURCE=.\Source\Ole2ui.r

!IF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Release"


"$(INTDIR)\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /I "..\uilib\source" /D "NDEBUG" /D "_MAC"\
 /D "_MSC_VER" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Simpcntr - Macintosh SimpleContainer Debug"


"$(INTDIR)\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /I "..\uilib\source" /D "_DEBUG" /D "_MAC"\
 /D "_MSC_VER" /D "DEBUG" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
