# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=Skel - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to Skel - Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "Skel - Macintosh Release" && "$(CFG)" !=\
 "Skel - Macintosh Debug" && "$(CFG)" != "Skel - Power Macintosh Release" &&\
 "$(CFG)" != "Skel - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "skel.mak" CFG="Skel - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Skel - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "Skel - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "Skel - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "Skel - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "Skel - Power Macintosh Debug"

!IF  "$(CFG)" == "Skel - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir MacRel
# PROP Intermediate_Dir MacRel
# PROP Target_Dir
OUTDIR=.\MacRel
INTDIR=.\MacRel

ALL : "$(OUTDIR)\skel.exe" "$(OUTDIR)\skel.trg"

CLEAN : 
	-@erase ".\MacRel\skel.trg"
	-@erase ".\MacRel\skel.exe"
	-@erase ".\MacRel\skel.obj"
	-@erase ".\MacRel\transkel.obj"
	-@erase ".\MacRel\skel.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/skel.pch" /YX /Fo"$(INTDIR)/" /c 
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
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/skel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SKL1" /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SKL1"\
 /pdb:"$(OUTDIR)/skel.pdb" /machine:M68K /out:"$(OUTDIR)/skel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/skel.obj" \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/skel.rsc"

"$(OUTDIR)\skel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/skel.exe"

"$(OUTDIR)\skel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDbg
# PROP BASE Intermediate_Dir MacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir MacDbg
# PROP Intermediate_Dir MacDbg
# PROP Target_Dir
OUTDIR=.\MacDbg
INTDIR=.\MacDbg

ALL : "$(OUTDIR)\skel.exe" "$(OUTDIR)\skel.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\skel.pdb"
	-@erase ".\MacDbg\skel.obj"
	-@erase ".\MacDbg\transkel.obj"
	-@erase ".\MacDbg\skel.rsc"
	-@erase ".\MacDbg\skel.trg"
	-@erase ".\MacDbg\skel.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mac
# ADD MTL /nologo /D "_DEBUG" /mac
MTL_PROJ=/nologo /D "_DEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "_DEBUG" /YX /c
CPP_PROJ=/nologo /AL /Q68s /Q68m /W3 /GX /Zi /Od /D "_WINDOWS" /D "_MAC" /D\
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/skel.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
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
MRC=mrc.exe
# ADD BASE MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_68K_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/skel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="SKL1" /debug /machine:M68K
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="SKL1"\
 /pdb:"$(OUTDIR)/skel.pdb" /debug /machine:M68K /out:"$(OUTDIR)/skel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/skel.obj" \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/skel.rsc"

"$(OUTDIR)\skel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/skel.exe"

"$(OUTDIR)\skel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir PMacRel
# PROP BASE Intermediate_Dir PMacRel
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir PMacRel
# PROP Intermediate_Dir PMacRel
# PROP Target_Dir
OUTDIR=.\PMacRel
INTDIR=.\PMacRel

ALL : "$(OUTDIR)\skel.exe" "$(OUTDIR)\skel.trg"

CLEAN : 
	-@erase ".\PMacRel\skel.trg"
	-@erase ".\PMacRel\skel.exe"
	-@erase ".\PMacRel\skel.obj"
	-@erase ".\PMacRel\transkel.obj"
	-@erase ".\PMacRel\skel.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "NDEBUG" /Fp"$(INTDIR)/skel.pch" /YX /Fo"$(INTDIR)/" /c 
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
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "NDEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/skel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SKL1" /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SKL1"\
 /incremental:no /pdb:"$(OUTDIR)/skel.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/skel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/skel.obj" \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/skel.rsc"

"$(OUTDIR)\skel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/skel.exe"

"$(OUTDIR)\skel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir PMacDbg
# PROP BASE Intermediate_Dir PMacDbg
# PROP BASE Target_Dir
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir PMacDbg
# PROP Intermediate_Dir PMacDbg
# PROP Target_Dir
OUTDIR=.\PMacDbg
INTDIR=.\PMacDbg

ALL : "$(OUTDIR)\skel.exe" "$(OUTDIR)\skel.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\skel.ilk"
	-@erase ".\PMacDbg\transkel.obj"
	-@erase ".\PMacDbg\skel.obj"
	-@erase ".\PMacDbg\skel.rsc"
	-@erase ".\PMacDbg\skel.pdb"
	-@erase ".\PMacDbg\skel.trg"
	-@erase ".\PMacDbg\skel.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG"
# ADD MTL /nologo /D "_DEBUG"
MTL_PROJ=/nologo /D "_DEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /YX /c
CPP_PROJ=/nologo /W3 /GX /Zi /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32"\
 /D "_DEBUG" /Fp"$(INTDIR)/skel.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
MRC=mrc.exe
# ADD BASE MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
# ADD MRC /D "_MPPC_" /D "_MAC" /D "_DEBUG" /NOLOGO
MRC_PROJ=/D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409 /NOLOGO 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/skel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="SKL1" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="SKL1"\
 /incremental:yes /pdb:"$(OUTDIR)/skel.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/skel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/skel.obj" \
	"$(INTDIR)/skel.rsc"

"$(OUTDIR)\skel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/skel.exe"

"$(OUTDIR)\skel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\skel.exe\
 "$(MFILE32_DEST):skel.exe">"$(OUTDIR)\skel.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "Skel - Macintosh Release"
# Name "Skel - Macintosh Debug"
# Name "Skel - Power Macintosh Release"
# Name "Skel - Power Macintosh Debug"

!IF  "$(CFG)" == "Skel - Macintosh Release"

!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\transkel.c
DEP_CPP_TRANS=\
	".\transkel.h"\
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
	{$(INCLUDE)}"\macos\Dialogs.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	

!IF  "$(CFG)" == "Skel - Macintosh Release"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\skel.c
DEP_CPP_SKEL_=\
	".\transkel.h"\
	{$(INCLUDE)}"\macos\Dialogs.h"\
	{$(INCLUDE)}"\macos\Windows.h"\
	{$(INCLUDE)}"\macos\Memory.h"\
	{$(INCLUDE)}"\macos\Menus.h"\
	{$(INCLUDE)}"\macos\QuickDra.h"\
	{$(INCLUDE)}"\macos\ToolUtil.h"\
	{$(INCLUDE)}"\macos\msvcmac.h"\
	

!IF  "$(CFG)" == "Skel - Macintosh Release"


"$(INTDIR)\skel.obj" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"


"$(INTDIR)\skel.obj" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"


"$(INTDIR)\skel.obj" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"


"$(INTDIR)\skel.obj" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_SKEL_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\skel.r

!IF  "$(CFG)" == "Skel - Macintosh Release"


"$(INTDIR)\skel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/skel.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"


"$(INTDIR)\skel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/skel.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"


"$(INTDIR)\skel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/skel.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"


"$(INTDIR)\skel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/skel.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "Skel - Macintosh Release"

!ELSEIF  "$(CFG)" == "Skel - Macintosh Debug"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "Skel - Power Macintosh Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
