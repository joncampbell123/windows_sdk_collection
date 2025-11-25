# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=SiCoPPC - Power Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to SiCoPPC - Power Macintosh\
 Debug.
!ENDIF 

!IF "$(CFG)" != "SiCoPPC - Power Macintosh Release" && "$(CFG)" !=\
 "SiCoPPC - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SiCoPPC.mak" CFG="SiCoPPC - Power Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SiCoPPC - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "SiCoPPC - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "SiCoPPC - Power Macintosh Debug"
MRC=mrc.exe
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMCRel
# PROP BASE Intermediate_Dir PMCRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMCRel
# PROP Intermediate_Dir PMCRel
OUTDIR=.\PMCRel
INTDIR=.\PMCRel

ALL : ".\PMCRel\SiCoPPC.exe" ".\PMCRel\SiCoPPC.trg"

CLEAN : 
	-@erase ".\PMCRel\SiCoPPC.trg"
	-@erase ".\PMCRel\SiCoPPC.exe"
	-@erase ".\PMCRel\precomp.obj"
	-@erase ".\PMCRel\cerrors.obj"
	-@erase ".\PMCRel\aevents.obj"
	-@erase ".\PMCRel\main.obj"
	-@erase ".\PMCRel\ias.obj"
	-@erase ".\PMCRel\site.obj"
	-@erase ".\PMCRel\util.obj"
	-@erase ".\PMCRel\Imf.obj"
	-@erase ".\PMCRel\cwindow.obj"
	-@erase ".\PMCRel\App.obj"
	-@erase ".\PMCRel\iocs.obj"
	-@erase ".\PMCRel\debug.obj"
	-@erase ".\PMCRel\Ole2ui.rsc"
	-@erase ".\PMCRel\simpcntr.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/SiCoPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /Zp2 /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_WINDOWS" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "WINGS" /D "_M_MPPC_" /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "NDEBUG" /D __pascal= /D _pascal= /D pascal= /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "WINGS" /D\
 "_M_MPPC_" /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D\
 "NDEBUG" /D __pascal= /D _pascal= /D pascal= /Fp"$(INTDIR)/SiCoPPC.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMCRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG"
# ADD MRC /I "c:\msvc30\mac\include\mrc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /D "_PPCMAC"
MRC_PROJ=/I "c:\msvc30\mac\include\mrc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /D\
 "_PPCMAC" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SiCoPPC.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="SiCo" /machine:MPPC /pdb:"PMCRel/SiCoPPC.pdb"/machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="SiCo" /incremental:no\
 /pdb:"$(OUTDIR)/SiCoPPC.pdb" /machine:MPPC /out:"$(OUTDIR)/SiCoPPC.exe"\
 /pdb:"PMCRel/SiCoPPC.pdb"/machine:MPPC 
LINK32_OBJS= \
	".\PMCRel\precomp.obj" \
	".\PMCRel\cerrors.obj" \
	".\PMCRel\aevents.obj" \
	".\PMCRel\main.obj" \
	".\PMCRel\ias.obj" \
	".\PMCRel\site.obj" \
	".\PMCRel\util.obj" \
	".\PMCRel\Imf.obj" \
	".\PMCRel\cwindow.obj" \
	".\PMCRel\App.obj" \
	".\PMCRel\iocs.obj" \
	".\PMCRel\debug.obj" \
	".\PMCRel\Ole2ui.rsc" \
	".\PMCRel\simpcntr.rsc"

".\PMCRel\SiCoPPC.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMCRel\SiCoPPC.exe"

".\PMCRel\SiCoPPC.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCRel\SiCoPPC.exe\
 "$(MFILE32_DEST)":SiCoPPC.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCRel\SiCoPPC.exe\
 "$(MFILE32_DEST)":SiCoPPC.exe

!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMCDebug
# PROP BASE Intermediate_Dir PMCDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMCDebug
# PROP Intermediate_Dir PMCDebug
OUTDIR=.\PMCDebug
INTDIR=.\PMCDebug

ALL : ".\PMCDebug\SiCoPPC.exe" ".\PMCDebug\SiCoPPC.trg"

CLEAN : 
	-@erase ".\PMCDebug\vc40.pdb"
	-@erase ".\PMCDebug\SiCoPPC.ilk"
	-@erase ".\PMCDebug\debug.obj"
	-@erase ".\PMCDebug\iocs.obj"
	-@erase ".\PMCDebug\ias.obj"
	-@erase ".\PMCDebug\cwindow.obj"
	-@erase ".\PMCDebug\main.obj"
	-@erase ".\PMCDebug\site.obj"
	-@erase ".\PMCDebug\precomp.obj"
	-@erase ".\PMCDebug\cerrors.obj"
	-@erase ".\PMCDebug\aevents.obj"
	-@erase ".\PMCDebug\Imf.obj"
	-@erase ".\PMCDebug\App.obj"
	-@erase ".\PMCDebug\util.obj"
	-@erase ".\PMCDebug\simpcntr.rsc"
	-@erase ".\PMCDebug\Ole2ui.rsc"
	-@erase ".\PMCDebug\SiCoPPC.pdb"
	-@erase ".\PMCDebug\SiCoPPC.trg"
	-@erase ".\PMCDebug\SiCoPPC.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/SiCoPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
# ADD BASE CPP /nologo /Zp2 /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_WINDOWS" /D "_DEBUG" /FR /YX /c
# ADD CPP /Zp2 /GX /Zi /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D "WINGS" /D "_M_MPPC_" /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_DEBUG" /D pascal= /D _pascal= /D __pascal= /YX /c
CPP_PROJ=/Zp2 /GX /Zi /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D "WINGS" /D\
 "_M_MPPC_" /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D\
 "_DEBUG" /D pascal= /D _pascal= /D __pascal= /Fp"$(INTDIR)/SiCoPPC.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMCDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG"
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /D "_PPCMAC"
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /D "_PPCMAC" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SiCoPPC.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 ole2d.lib ..\uilib\pmacdbg\ole2uid.lib /nologo /mac:bundle /mac:creator="SiCo" /debug /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2d.lib ..\uilib\pmacdbg\ole2uid.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="SiCo" /incremental:yes\
 /pdb:"$(OUTDIR)/SiCoPPC.pdb" /debug /machine:MPPC /out:"$(OUTDIR)/SiCoPPC.exe" 
LINK32_OBJS= \
	".\PMCDebug\debug.obj" \
	".\PMCDebug\iocs.obj" \
	".\PMCDebug\ias.obj" \
	".\PMCDebug\cwindow.obj" \
	".\PMCDebug\main.obj" \
	".\PMCDebug\site.obj" \
	".\PMCDebug\precomp.obj" \
	".\PMCDebug\cerrors.obj" \
	".\PMCDebug\aevents.obj" \
	".\PMCDebug\Imf.obj" \
	".\PMCDebug\App.obj" \
	".\PMCDebug\util.obj" \
	".\PMCDebug\simpcntr.rsc" \
	".\PMCDebug\Ole2ui.rsc"

".\PMCDebug\SiCoPPC.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	".\PMCDebug\SiCoPPC.exe"

".\PMCDebug\SiCoPPC.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCDebug\SiCoPPC.exe\
 "$(MFILE32_DEST)":SiCoPPC.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCDebug\SiCoPPC.exe\
 "$(MFILE32_DEST)":SiCoPPC.exe

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

# Name "SiCoPPC - Power Macintosh Release"
# Name "SiCoPPC - Power Macintosh Debug"

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\aevents.cpp
DEP_CPP_AEVEN=\
	".\Source\oleguid.h"\
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
	{$(INCLUDE)}"\macos\Windows.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
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
	".\Source\cwindow.h"\
	".\Source\site.h"\
	".\Source\cerrors.h"\
	".\Source\util.h"\
	".\Source\debug.h"\
	".\Source\ias.h"\
	".\Source\imf.h"\
	".\Source\iocs.h"\
	".\Source\mac.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_APP_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_CERRO=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\cerrors.obj" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\cerrors.obj" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_CERRO) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_CWIND=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_DEBUG=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IAS_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\ias.obj" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\ias.obj" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_IAS_C) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IMF_C=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\Imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\Imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_IOCS_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\iocs.obj" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\iocs.obj" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_IOCS_) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_MAIN_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\precomp.cpp
DEP_CPP_PRECO=\
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_PRECO=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\precomp.obj" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\precomp.obj" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\simpcntr.r

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\simpcntr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpcntr.rsc" /I "c:\msvc30\mac\include\mrc" /D "_MPPC_"\
 /D "_MAC" /D "NDEBUG" /D "_PPCMAC" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


".\PMCDebug\simpcntr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpcntr.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /D\
 "_PPCMAC" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\site.cpp
DEP_CPP_SITE_=\
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_SITE_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\site.obj" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_SITE_) "$(INTDIR)"
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
	{$(INCLUDE)}"\macos\Windows.h"\
	".\Source\Mycoguid.h"\
	".\Source\scodes.h"\
	
NODEP_CPP_UTIL_=\
	".\Source\oleguid.h"\
	

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMCDebug\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

".\PMCDebug\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Ole2ui.r

!IF  "$(CFG)" == "SiCoPPC - Power Macintosh Release"


".\PMCRel\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /I "c:\msvc30\mac\include\mrc" /D "_MPPC_"\
 /D "_MAC" /D "NDEBUG" /D "_PPCMAC" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "SiCoPPC - Power Macintosh Debug"


".\PMCDebug\Ole2ui.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Ole2ui.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /D\
 "_PPCMAC" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
