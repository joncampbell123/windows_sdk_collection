# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=SwapSkel - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to SwapSkel - Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "SwapSkel - Macintosh Release" && "$(CFG)" !=\
 "SwapSkel - Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SwapSkel1.mak" CFG="SwapSkel - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SwapSkel - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "SwapSkel - Macintosh Debug" (based on "Macintosh Application")
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
MRC=mrc.exe
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\SwapSkel1.exe" "$(OUTDIR)\SwapSkel1.trg"

CLEAN : 
	-@erase ".\MacRel\SwapSkel1.trg"
	-@erase ".\MacRel\SwapSkel1.exe"
	-@erase ".\MacRel\Transkel.obj"
	-@erase ".\MacRel\Msklhelp.obj"
	-@erase ".\MacRel\Growzone.obj"
	-@erase ".\MacRel\Msklzoom.obj"
	-@erase ".\MacRel\Swapskel.obj"
	-@erase ".\MacRel\Msklrgn.obj"
	-@erase ".\MacRel\Mskledit.obj"
	-@erase ".\MacRel\Multskel.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/SwapSkel1.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "NDEBUG"
# ADD BASE MRC /D "_MAC" /D "NDEBUG"
# ADD MRC /D "_MAC" /D "NDEBUG"
MRC_PROJ=/D "_MAC" /D "NDEBUG" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SwapSkel1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 swap.lib sanes.lib wlm.lib wlm.rsc commdlg.rsc nafxcm.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /nologo /MAC:bundle /machine:M68K /nodefaultlib:"swapd.lib"
# ADD LINK32 swap.lib interfac.lib /nologo /MAC:bundle /MAC:creator="SWAP" /machine:M68K /nodefaultlib:"swapd.lib"
LINK32_FLAGS=swap.lib interfac.lib /nologo /MAC:bundle /MAC:type="APPL"\
 /MAC:creator="SWAP" /pdb:"$(OUTDIR)/SwapSkel1.pdb" /machine:M68K\
 /nodefaultlib:"swapd.lib" /out:"$(OUTDIR)/SwapSkel1.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Transkel.obj" \
	"$(INTDIR)/Msklhelp.obj" \
	"$(INTDIR)/Growzone.obj" \
	"$(INTDIR)/Msklzoom.obj" \
	"$(INTDIR)/Swapskel.obj" \
	"$(INTDIR)/Msklrgn.obj" \
	"$(INTDIR)/Mskledit.obj" \
	"$(INTDIR)/Multskel.rsc"

"$(OUTDIR)\SwapSkel1.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SwapSkel1.exe"

"$(OUTDIR)\SwapSkel1.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\SwapSkel1.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\SwapSkel1.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDebug
# PROP BASE Intermediate_Dir MacDebug
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDebug
# PROP Intermediate_Dir MacDebug
OUTDIR=.\MacDebug
INTDIR=.\MacDebug

ALL : "$(OUTDIR)\SwapSkel1.exe" "$(OUTDIR)\SwapSkel1.trg"

CLEAN : 
	-@erase ".\MacDebug\vc40.pdb"
	-@erase ".\MacDebug\SwapSkel1.trg"
	-@erase ".\MacDebug\SwapSkel1.exe"
	-@erase ".\MacDebug\Transkel.obj"
	-@erase ".\MacDebug\Msklhelp.obj"
	-@erase ".\MacDebug\Growzone.obj"
	-@erase ".\MacDebug\Msklzoom.obj"
	-@erase ".\MacDebug\Swapskel.obj"
	-@erase ".\MacDebug\Msklrgn.obj"
	-@erase ".\MacDebug\Mskledit.obj"
	-@erase ".\MacDebug\Multskel.rsc"
	-@erase ".\MacDebug\SwapSkel1.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/SwapSkel1.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\MacDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD RSC /l 0x409 /r /d "_MAC" /d "_68K_" /d "_DEBUG"
# ADD BASE MRC /D "_MAC" /D "_DEBUG"
# ADD MRC /D "_MAC" /D "_DEBUG"
MRC_PROJ=/D "_MAC" /D "_DEBUG" /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BASE BSC32 /Iu
# ADD BSC32 /nologo
# SUBTRACT BSC32 /Iu
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SwapSkel1.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 swapd.lib sanes.lib wlmd.lib wlm.rsc commdlg.rsc nafxcmd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /nologo /MAC:bundle /debug /machine:M68K /nodefaultlib:"swap.lib"
# ADD LINK32 swapd.lib interfac.lib /nologo /MAC:bundle /MAC:creator="SWAP" /debug /machine:M68K /nodefaultlib:"swap.lib"
LINK32_FLAGS=swapd.lib interfac.lib /nologo /MAC:bundle /MAC:type="APPL"\
 /MAC:creator="SWAP" /pdb:"$(OUTDIR)/SwapSkel1.pdb" /debug /machine:M68K\
 /nodefaultlib:"swap.lib" /out:"$(OUTDIR)/SwapSkel1.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Transkel.obj" \
	"$(INTDIR)/Msklhelp.obj" \
	"$(INTDIR)/Growzone.obj" \
	"$(INTDIR)/Msklzoom.obj" \
	"$(INTDIR)/Swapskel.obj" \
	"$(INTDIR)/Msklrgn.obj" \
	"$(INTDIR)/Mskledit.obj" \
	"$(INTDIR)/Multskel.rsc"

"$(OUTDIR)\SwapSkel1.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SwapSkel1.exe"

"$(OUTDIR)\SwapSkel1.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDebug\SwapSkel1.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDebug\SwapSkel1.exe "$(MFILE32_DEST)"

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

# Name "SwapSkel - Macintosh Release"
# Name "SwapSkel - Macintosh Debug"

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"

!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Transkel.c
DEP_CPP_TRANS=\
	".\Transkel.h"\
	{$(INCLUDE)}"\macos\Desk.h"\
	{$(INCLUDE)}"\macos\DiskInit.h"\
	{$(INCLUDE)}"\macos\Events.h"\
	{$(INCLUDE)}"\macos\Fonts.h"\
	{$(INCLUDE)}"\macos\Memory.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\OsEvents.h"\
	{$(INCLUDE)}"\macos\QuickDra.h"\
	{$(INCLUDE)}"\macos\Resource.h"\
	{$(INCLUDE)}"\macos\ToolUtil.h"\
	{$(INCLUDE)}"\macos\dialogs.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msklhelp.c
DEP_CPP_MSKLH=\
	".\Multskel.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	{$(INCLUDE)}"\macos\Controls.h"\
	{$(INCLUDE)}"\macos\Memory.h"\
	{$(INCLUDE)}"\macos\Resource.h"\
	{$(INCLUDE)}"\macos\TextEdit.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Multskel.r

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Multskel.rsc" /D "_MAC" /D "NDEBUG" /l 0x409 $(SOURCE)


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/Multskel.rsc" /D "_MAC" /D "_DEBUG" /l 0x409 $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Growzone.c
DEP_CPP_GROWZ=\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"

# SUBTRACT CPP /Q68s

"$(INTDIR)\Growzone.obj" : $(SOURCE) $(DEP_CPP_GROWZ) "$(INTDIR)"
   $(CPP) /nologo /AL /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)/SwapSkel1.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE)


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"

# SUBTRACT CPP /Q68s

BuildCmds= \
	$(CPP) /nologo /AL /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "_DEBUG" /D "_MBCS" /Fp"$(INTDIR)/SwapSkel1.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\Growzone.obj" : $(SOURCE) $(DEP_CPP_GROWZ) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_GROWZ) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msklzoom.c
DEP_CPP_MSKLZ=\
	".\Multskel.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Swapskel.c
DEP_CPP_SWAPS=\
	".\Growzone.h"\
	".\Multskel.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	".\Transkel.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Swapskel.obj" : $(SOURCE) $(DEP_CPP_SWAPS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Swapskel.obj" : $(SOURCE) $(DEP_CPP_SWAPS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SWAPS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Msklrgn.c
DEP_CPP_MSKLR=\
	".\Multskel.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	{$(INCLUDE)}"\macos\Events.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mskledit.c
DEP_CPP_MSKLE=\
	".\Multskel.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	{$(INCLUDE)}"\macos\Scrap.h"\
	{$(INCLUDE)}"\macos\TextEdit.h"\
	

!IF  "$(CFG)" == "SwapSkel - Macintosh Release"


"$(INTDIR)\Mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "SwapSkel - Macintosh Debug"


"$(INTDIR)\Mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
