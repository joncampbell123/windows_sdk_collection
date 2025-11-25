# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Application" 0x0401

!IF "$(CFG)" == ""
CFG=Simple Server PPC - Power Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to Simple Server PPC - Power\
 Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "Simple Server PPC - Power Macintosh Release" && "$(CFG)" !=\
 "Simple Server PPC - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SiSePPC.mak" CFG="Simple Server PPC - Power Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Simple Server PPC - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Simple Server PPC - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Simple Server PPC - Power Macintosh Debug"
MRC=mrc.exe
MTL=mktyplib.exe
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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

ALL : "$(OUTDIR)\SiSePPC.exe" "$(OUTDIR)\SiSePPC.trg"

CLEAN : 
	-@erase ".\PMCRel\SiSePPC.trg"
	-@erase ".\PMCRel\SiSePPC.exe"
	-@erase ".\PMCRel\cobject.obj"
	-@erase ".\PMCRel\imf.obj"
	-@erase ".\PMCRel\precomp.obj"
	-@erase ".\PMCRel\ido.obj"
	-@erase ".\PMCRel\App.obj"
	-@erase ".\PMCRel\Aevents.obj"
	-@erase ".\PMCRel\util.obj"
	-@erase ".\PMCRel\ctoolbar.obj"
	-@erase ".\PMCRel\oleerror.obj"
	-@erase ".\PMCRel\ioo.obj"
	-@erase ".\PMCRel\doc.obj"
	-@erase ".\PMCRel\Ips.obj"
	-@erase ".\PMCRel\icf.obj"
	-@erase ".\PMCRel\cshape.obj"
	-@erase ".\PMCRel\main.obj"
	-@erase ".\PMCRel\Cwindow.obj"
	-@erase ".\PMCRel\debug.obj"
	-@erase ".\PMCRel\simpsvr.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/SiSePPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
# ADD BASE CPP /nologo /Zp2 /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_WINDOWS" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_M_MPPC_" /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "NDEBUG" /D __pascal= /D _pascal= /D pascal= /YX /c
CPP_PROJ=/nologo /Zp2 /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_M_MPPC_"\
 /D "_PPCMAC" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "NDEBUG" /D\
 __pascal= /D _pascal= /D pascal= /Fp"$(INTDIR)/SiSePPC.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\PMCRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "NDEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG"
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /D "_PPCMAC"
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /D "_PPCMAC" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SiSePPC.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle /mac:creator="SiSe" /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2.lib ..\uilib\pmacrel\ole2uir.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="SiSe" /incremental:no\
 /pdb:"$(OUTDIR)/SiSePPC.pdb" /machine:MPPC /out:"$(OUTDIR)/SiSePPC.exe" 
LINK32_OBJS= \
	"$(INTDIR)/cobject.obj" \
	"$(INTDIR)/imf.obj" \
	"$(INTDIR)/precomp.obj" \
	"$(INTDIR)/ido.obj" \
	"$(INTDIR)/App.obj" \
	"$(INTDIR)/Aevents.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/ctoolbar.obj" \
	"$(INTDIR)/oleerror.obj" \
	"$(INTDIR)/ioo.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/Ips.obj" \
	"$(INTDIR)/icf.obj" \
	"$(INTDIR)/cshape.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/Cwindow.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/simpsvr.rsc"

"$(OUTDIR)\SiSePPC.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SiSePPC.exe"

"$(OUTDIR)\SiSePPC.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCRel\SiSePPC.exe\
 "$(MFILE32_DEST)":SiSePPC.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCRel\SiSePPC.exe\
 "$(MFILE32_DEST)":SiSePPC.exe

!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\SiSePPC.exe" "$(OUTDIR)\SiSePPC.trg"

CLEAN : 
	-@erase ".\PMCDebug\vc40.pdb"
	-@erase ".\PMCDebug\SiSePPC.ilk"
	-@erase ".\PMCDebug\Cwindow.obj"
	-@erase ".\PMCDebug\util.obj"
	-@erase ".\PMCDebug\ioo.obj"
	-@erase ".\PMCDebug\doc.obj"
	-@erase ".\PMCDebug\debug.obj"
	-@erase ".\PMCDebug\Ips.obj"
	-@erase ".\PMCDebug\icf.obj"
	-@erase ".\PMCDebug\cshape.obj"
	-@erase ".\PMCDebug\cobject.obj"
	-@erase ".\PMCDebug\precomp.obj"
	-@erase ".\PMCDebug\ctoolbar.obj"
	-@erase ".\PMCDebug\oleerror.obj"
	-@erase ".\PMCDebug\Aevents.obj"
	-@erase ".\PMCDebug\main.obj"
	-@erase ".\PMCDebug\imf.obj"
	-@erase ".\PMCDebug\ido.obj"
	-@erase ".\PMCDebug\App.obj"
	-@erase ".\PMCDebug\simpsvr.rsc"
	-@erase ".\PMCDebug\SiSePPC.pdb"
	-@erase ".\PMCDebug\SiSePPC.trg"
	-@erase ".\PMCDebug\SiSePPC.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/SiSePPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
# ADD BASE CPP /nologo /Zp2 /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_WINDOWS" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /GX /Zi /Od /FI"macole\msvcole.h" /D "DEBUG" /D "_MSC_VER" /D "_PPCMAC" /D "_M_MPPC_" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D "_POWERPC_" /D "_DEBUG" /D _pascal= /D __pascal= /D pascal= /c
CPP_PROJ=/nologo /Zp2 /GX /Zi /Od /FI"macole\msvcole.h" /D "DEBUG" /D\
 "_MSC_VER" /D "_PPCMAC" /D "_M_MPPC_" /D "_MAC" /D "_MPPC_" /D "_PPC_" /D\
 "_POWERPC_" /D "_DEBUG" /D _pascal= /D __pascal= /D pascal= /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMCDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_MPPC_" /d "_DEBUG"
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG"
# ADD MRC /I "c:\msvc30\mac\include\mrc" /D "_PPCMAC" /D "_MPPC_" /D "_MAC" /D "_DEBUG"
MRC_PROJ=/I "c:\msvc30\mac\include\mrc" /D "_PPCMAC" /D "_MPPC_" /D "_MAC" /D\
 "_DEBUG" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SiSePPC.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.rsc commdlg.rsc cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 ole2d.lib ..\uilib\pmacdbg\ole2uid.lib /nologo /mac:bundle /mac:creator="SiSe" /debug /machine:MPPC
# SUBTRACT LINK32 /pdb:none
LINK32_FLAGS=ole2d.lib ..\uilib\pmacdbg\ole2uid.lib /nologo /mac:bundle\
 /mac:type="APPL" /mac:creator="SiSe" /incremental:yes\
 /pdb:"$(OUTDIR)/SiSePPC.pdb" /debug /machine:MPPC /out:"$(OUTDIR)/SiSePPC.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Cwindow.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/ioo.obj" \
	"$(INTDIR)/doc.obj" \
	"$(INTDIR)/debug.obj" \
	"$(INTDIR)/Ips.obj" \
	"$(INTDIR)/icf.obj" \
	"$(INTDIR)/cshape.obj" \
	"$(INTDIR)/cobject.obj" \
	"$(INTDIR)/precomp.obj" \
	"$(INTDIR)/ctoolbar.obj" \
	"$(INTDIR)/oleerror.obj" \
	"$(INTDIR)/Aevents.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/imf.obj" \
	"$(INTDIR)/ido.obj" \
	"$(INTDIR)/App.obj" \
	"$(INTDIR)/simpsvr.rsc"

"$(OUTDIR)\SiSePPC.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SiSePPC.exe"

"$(OUTDIR)\SiSePPC.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCDebug\SiSePPC.exe\
 "$(MFILE32_DEST)":SiSePPC.exe

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMCDebug\SiSePPC.exe\
 "$(MFILE32_DEST)":SiSePPC.exe

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

# Name "Simple Server PPC - Power Macintosh Release"
# Name "Simple Server PPC - Power Macintosh Debug"

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\Aevents.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\Aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Aevents.obj" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_AEVEN) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\App.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\App.obj" : $(SOURCE) $(DEP_CPP_APP_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	".\Source\Mycoguid.h"\
	

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

SOURCE=.\Source\cobject.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\cobject.obj" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cobject.obj" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_COBJE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\cshape.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\cshape.obj" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\cshape.obj" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CSHAP) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ctoolbar.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\ctoolbar.obj" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ctoolbar.obj" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CTOOL) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Cwindow.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\Cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Cwindow.obj" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_CWIND) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\debug.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\debug.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

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

SOURCE=.\Source\doc.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\doc.obj" : $(SOURCE) $(DEP_CPP_DOC_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

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

SOURCE=.\Source\icf.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\icf.obj" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\icf.obj" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_ICF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ido.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\ido.obj" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ido.obj" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IDO_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\imf.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\imf.obj" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IMF_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ioo.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\ioo.obj" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\ioo.obj" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IOO_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Ips.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\Ips.obj" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\Ips.obj" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_IPS_C) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\main.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

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

SOURCE=.\Source\oleerror.cpp

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"

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
	

"$(INTDIR)\oleerror.obj" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"

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
	".\macos\WINDOWS.H"\
	

BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\oleerror.obj" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEER) "$(INTDIR)"
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
	".\macos\WINDOWS.H"\
	

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"


"$(INTDIR)\precomp.obj" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\precomp.obj" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_PRECO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\simpsvr.r

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"


"$(INTDIR)\simpsvr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpsvr.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /D\
 "_PPCMAC" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"


"$(INTDIR)\simpsvr.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/simpsvr.rsc" /I "c:\msvc30\mac\include\mrc" /D "_PPCMAC"\
 /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 $(SOURCE)


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
	".\macos\WINDOWS.H"\
	

!IF  "$(CFG)" == "Simple Server PPC - Power Macintosh Release"


"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Simple Server PPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
