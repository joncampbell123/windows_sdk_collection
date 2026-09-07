# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=SaCode - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to SaCode - Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "SaCode - Macintosh Release" && "$(CFG)" !=\
 "SaCode - Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "SaCode.mak" CFG="SaCode - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SaCode - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "SaCode - Macintosh Debug" (based on "Macintosh Application")
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
# PROP Target_Last_Scanned "SaCode - Macintosh Debug"
MRC=mrc.exe
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "SaCode - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\SaCode.exe" "$(OUTDIR)\SaCode.trg"

CLEAN : 
	-@erase ".\MacRel\SaCode.trg"
	-@erase ".\MacRel\SaCode.exe"
	-@erase ".\MacRel\Main.obj"
	-@erase ".\MacRel\Init.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /W3 /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /W3 /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /D\
 "_MBCS" /Fp"$(INTDIR)/SaCode.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SaCode.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 swap.lib sanes.lib wlm.lib aslm.lib wlm.rsc commdlg.rsc nafxcm.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /nologo /MAC:bundle /machine:M68K /nodefaultlib:"swapd.lib"
# ADD LINK32 /nologo /MAC:bundle /MAC:type="INIT" /MAC:creator="SACO" /machine:M68K /nodefaultlib:"swapd.lib" /nodefaultlib /entry:main /section:sacode,,resource="INIT"@1
# SUBTRACT LINK32 /pdb:none /map
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="INIT" /MAC:creator="SACO"\
 /pdb:"$(OUTDIR)/SaCode.pdb" /machine:M68K /nodefaultlib:"swapd.lib"\
 /nodefaultlib /out:"$(OUTDIR)/SaCode.exe" /entry:main\
 /section:sacode,,resource="INIT"@1 
LINK32_OBJS= \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Init.obj"

"$(OUTDIR)\SaCode.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SaCode.exe"

"$(OUTDIR)\SaCode.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\SaCode.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\SaCode.exe "$(MFILE32_DEST)"

!ELSEIF  "$(CFG)" == "SaCode - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDebug
# PROP BASE Intermediate_Dir MacDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDebug
# PROP Intermediate_Dir MacDebug
OUTDIR=.\MacDebug
INTDIR=.\MacDebug

ALL : "$(OUTDIR)\SaCode.exe" "$(OUTDIR)\SaCode.trg"

CLEAN : 
	-@erase ".\MacDebug\SaCode.map"
	-@erase ".\MacDebug\Main.obj"
	-@erase ".\MacDebug\Init.obj"
	-@erase ".\MacDebug\SaCode.trg"
	-@erase ".\MacDebug\SaCode.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
# ADD BASE CPP /nologo /AL /Gt1 /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /FR /YX /c
# ADD CPP /nologo /Q68m /W3 /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /D "_MBCS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /Q68m /W3 /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG"\
 /D "_MBCS" /Fp"$(INTDIR)/SaCode.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/SaCode.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 swapd.lib sanes.lib wlmd.lib aslmd.lib wlm.rsc commdlg.rsc nafxcmd.lib mfcuia32.lib mfcans32.lib ole32.lib oleaut32.lib uuid.lib /nologo /MAC:bundle /debug /machine:M68K /nodefaultlib:"swap.lib"
# ADD LINK32 /nologo /MAC:bundle /MAC:type="INIT" /MAC:creator="SACO" /map /machine:M68K /nodefaultlib:"swap.lib" /nodefaultlib /entry:main /section:sacode,,resource="INIT"@1
# SUBTRACT LINK32 /pdb:none /debug
LINK32_FLAGS=/nologo /MAC:bundle /MAC:type="INIT" /MAC:creator="SACO"\
 /pdb:"$(OUTDIR)/SaCode.pdb" /map:"$(INTDIR)/SaCode.map" /machine:M68K\
 /nodefaultlib:"swap.lib" /nodefaultlib /out:"$(OUTDIR)/SaCode.exe" /entry:main\
 /section:sacode,,resource="INIT"@1 
LINK32_OBJS= \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Init.obj"

"$(OUTDIR)\SaCode.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/SaCode.exe"

"$(OUTDIR)\SaCode.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDebug\SaCode.exe "$(MFILE32_DEST)"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDebug\SaCode.exe "$(MFILE32_DEST)"

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

# Name "SaCode - Macintosh Release"
# Name "SaCode - Macintosh Debug"

!IF  "$(CFG)" == "SaCode - Macintosh Release"

!ELSEIF  "$(CFG)" == "SaCode - Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Init.c
DEP_CPP_INIT_=\
	{$(INCLUDE)}"\macos\Osutils.h"\
	

"$(INTDIR)\Init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Main.c

"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
