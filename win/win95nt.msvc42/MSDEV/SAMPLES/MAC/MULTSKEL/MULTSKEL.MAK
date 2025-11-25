# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Application" 0x0401
# TARGTYPE "Macintosh Application" 0x0301

!IF "$(CFG)" == ""
CFG=MultSkel - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to MultSkel - Macintosh Debug.
!ENDIF 

!IF "$(CFG)" != "MultSkel - Macintosh Release" && "$(CFG)" !=\
 "MultSkel - Macintosh Debug" && "$(CFG)" !=\
 "MultSkel - Power Macintosh Release" && "$(CFG)" !=\
 "MultSkel - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "multskel.mak" CFG="MultSkel - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MultSkel - Macintosh Release" (based on "Macintosh Application")
!MESSAGE "MultSkel - Macintosh Debug" (based on "Macintosh Application")
!MESSAGE "MultSkel - Power Macintosh Release" (based on\
 "Power Macintosh Application")
!MESSAGE "MultSkel - Power Macintosh Debug" (based on\
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
# PROP Target_Last_Scanned "MultSkel - Power Macintosh Debug"

!IF  "$(CFG)" == "MultSkel - Macintosh Release"

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

ALL : "$(OUTDIR)\multskel.exe" "$(OUTDIR)\multskel.trg"

CLEAN : 
	-@erase ".\MacRel\multskel.trg"
	-@erase ".\MacRel\multskel.exe"
	-@erase ".\MacRel\msklhelp.obj"
	-@erase ".\MacRel\mskledit.obj"
	-@erase ".\MacRel\transkel.obj"
	-@erase ".\MacRel\multskel.obj"
	-@erase ".\MacRel\msklzoom.obj"
	-@erase ".\MacRel\msklrgn.obj"
	-@erase ".\MacRel\multskel.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mac
# ADD MTL /nologo /D "NDEBUG" /mac
MTL_PROJ=/nologo /D "NDEBUG" /mac 
CPP=cl.exe
# ADD BASE CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "WIN32" /D "NDEBUG" /YX /c
# SUBTRACT CPP /WX /Fr
CPP_PROJ=/nologo /AL /Q68s /W3 /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D\
 "WIN32" /D "NDEBUG" /Fp"$(INTDIR)/multskel.pch" /YX /Fo"$(INTDIR)/" /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multskel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="MULT" /machine:M68K
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MULT"\
 /pdb:"$(OUTDIR)/multskel.pdb" /machine:M68K /out:"$(OUTDIR)/multskel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/msklhelp.obj" \
	"$(INTDIR)/mskledit.obj" \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/multskel.obj" \
	"$(INTDIR)/msklzoom.obj" \
	"$(INTDIR)/msklrgn.obj" \
	"$(INTDIR)/multskel.rsc"

"$(OUTDIR)\multskel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multskel.exe"

"$(OUTDIR)\multskel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacRel\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"

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

ALL : "$(OUTDIR)\multskel.exe" "$(OUTDIR)\multskel.trg"

CLEAN : 
	-@erase ".\MacDbg\vc40.pdb"
	-@erase ".\MacDbg\multskel.pdb"
	-@erase ".\MacDbg\msklrgn.obj"
	-@erase ".\MacDbg\msklhelp.obj"
	-@erase ".\MacDbg\transkel.obj"
	-@erase ".\MacDbg\mskledit.obj"
	-@erase ".\MacDbg\multskel.obj"
	-@erase ".\MacDbg\msklzoom.obj"
	-@erase ".\MacDbg\multskel.rsc"
	-@erase ".\MacDbg\multskel.trg"
	-@erase ".\MacDbg\multskel.exe"

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
 "_68K_" /D "WIN32" /D "_DEBUG" /Fp"$(INTDIR)/multskel.pch" /YX /Fo"$(INTDIR)/"\
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multskel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 wlm.lib /nologo /MAC:bundle /debug /machine:M68K
# ADD LINK32 wlm.lib /nologo /MAC:bundle /MAC:creator="MULT" /debug /machine:M68K
# SUBTRACT LINK32 /nodefaultlib /force
LINK32_FLAGS=wlm.lib /nologo /MAC:bundle /MAC:type="APPL" /MAC:creator="MULT"\
 /pdb:"$(OUTDIR)/multskel.pdb" /debug /machine:M68K\
 /out:"$(OUTDIR)/multskel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/msklrgn.obj" \
	"$(INTDIR)/msklhelp.obj" \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/mskledit.obj" \
	"$(INTDIR)/multskel.obj" \
	"$(INTDIR)/msklzoom.obj" \
	"$(INTDIR)/multskel.rsc"

"$(OUTDIR)\multskel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multskel.exe"

"$(OUTDIR)\multskel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\MacDbg\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"

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

ALL : "$(OUTDIR)\multskel.exe" "$(OUTDIR)\multskel.trg"

CLEAN : 
	-@erase ".\PMacRel\multskel.trg"
	-@erase ".\PMacRel\multskel.exe"
	-@erase ".\PMacRel\transkel.obj"
	-@erase ".\PMacRel\mskledit.obj"
	-@erase ".\PMacRel\multskel.obj"
	-@erase ".\PMacRel\msklrgn.obj"
	-@erase ".\PMacRel\msklzoom.obj"
	-@erase ".\PMacRel\msklhelp.obj"
	-@erase ".\PMacRel\multskel.rsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG"
# ADD MTL /nologo /D "NDEBUG"
MTL_PROJ=/nologo /D "NDEBUG" 
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D\
 "NDEBUG" /Fp"$(INTDIR)/multskel.pch" /YX /Fo"$(INTDIR)/" /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multskel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MULT" /machine:MPPC
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MULT"\
 /incremental:no /pdb:"$(OUTDIR)/multskel.pdb" /machine:MPPC\
 /out:"$(OUTDIR)/multskel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/mskledit.obj" \
	"$(INTDIR)/multskel.obj" \
	"$(INTDIR)/msklrgn.obj" \
	"$(INTDIR)/msklzoom.obj" \
	"$(INTDIR)/msklhelp.obj" \
	"$(INTDIR)/multskel.rsc"

"$(OUTDIR)\multskel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multskel.exe"

"$(OUTDIR)\multskel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacRel\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"

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

ALL : "$(OUTDIR)\multskel.exe" "$(OUTDIR)\multskel.trg"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\multskel.ilk"
	-@erase ".\PMacDbg\transkel.obj"
	-@erase ".\PMacDbg\mskledit.obj"
	-@erase ".\PMacDbg\multskel.obj"
	-@erase ".\PMacDbg\msklzoom.obj"
	-@erase ".\PMacDbg\msklrgn.obj"
	-@erase ".\PMacDbg\msklhelp.obj"
	-@erase ".\PMacDbg\multskel.rsc"
	-@erase ".\PMacDbg\multskel.pdb"
	-@erase ".\PMacDbg\multskel.trg"
	-@erase ".\PMacDbg\multskel.exe"

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
 /D "_DEBUG" /Fp"$(INTDIR)/multskel.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/multskel.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 cfrg.rsc /nologo /mac:bundle /debug /machine:MPPC
# ADD LINK32 /nologo /mac:bundle /mac:creator="MULT" /debug /machine:MPPC
LINK32_FLAGS=/nologo /mac:bundle /mac:type="APPL" /mac:creator="MULT"\
 /incremental:yes /pdb:"$(OUTDIR)/multskel.pdb" /debug /machine:MPPC\
 /out:"$(OUTDIR)/multskel.exe" 
LINK32_OBJS= \
	"$(INTDIR)/transkel.obj" \
	"$(INTDIR)/mskledit.obj" \
	"$(INTDIR)/multskel.obj" \
	"$(INTDIR)/msklzoom.obj" \
	"$(INTDIR)/msklrgn.obj" \
	"$(INTDIR)/msklhelp.obj" \
	"$(INTDIR)/multskel.rsc"

"$(OUTDIR)\multskel.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

MFILE32=mfile.exe
# ADD BASE MFILE32 COPY /NOLOGO
# ADD MFILE32 COPY /NOLOGO
MFILE32_FLAGS=COPY /NOLOGO 
MFILE32_FILES= \
	"$(OUTDIR)/multskel.exe"

"$(OUTDIR)\multskel.trg" : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

DOWNLOAD : "$(OUTDIR)" $(MFILE32_FILES)
    $(MFILE32) $(MFILE32_FLAGS) .\PMacDbg\multskel.exe\
 "$(MFILE32_DEST):multskel.exe">"$(OUTDIR)\multskel.trg"

!ENDIF 

################################################################################
# Begin Target

# Name "MultSkel - Macintosh Release"
# Name "MultSkel - Macintosh Debug"
# Name "MultSkel - Power Macintosh Release"
# Name "MultSkel - Power Macintosh Debug"

!IF  "$(CFG)" == "MultSkel - Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\multskel.r

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/multskel.rsc" /D "_68K_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/multskel.rsc" /D "_68K_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/multskel.rsc" /D "_MPPC_" /D "_MAC" /D "NDEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\multskel.rsc" : $(SOURCE) "$(INTDIR)"
   $(MRC) /o"$(INTDIR)/multskel.rsc" /D "_MPPC_" /D "_MAC" /D "_DEBUG" /l 0x409\
 /NOLOGO $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msklhelp.c
DEP_CPP_MSKLH=\
	".\multskel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	{$(INCLUDE)}"\macos\CONTROLS.H"\
	{$(INCLUDE)}"\macos\RESOURCE.H"\
	{$(INCLUDE)}"\macos\TEXTEDIT.H"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\msklhelp.obj" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLH) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msklrgn.c
DEP_CPP_MSKLR=\
	".\multskel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	{$(INCLUDE)}"\macos\EVENTS.H"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\msklrgn.obj" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\msklzoom.c
DEP_CPP_MSKLZ=\
	".\multskel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\msklzoom.obj" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLZ) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\multskel.c
DEP_CPP_MULTS=\
	".\multskel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	".\transkel.h"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\multskel.obj" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\multskel.obj" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\multskel.obj" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\multskel.obj" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MULTS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mskledit.c
DEP_CPP_MSKLE=\
	".\multskel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	{$(INCLUDE)}"\macos\SCRAP.H"\
	{$(INCLUDE)}"\macos\TEXTEDIT.H"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\mskledit.obj" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSKLE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt

!IF  "$(CFG)" == "MultSkel - Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\transkel.c
DEP_CPP_TRANS=\
	".\transkel.h"\
	{$(INCLUDE)}"\macos\DESK.H"\
	{$(INCLUDE)}"\macos\DISKINIT.H"\
	{$(INCLUDE)}"\macos\EVENTS.H"\
	{$(INCLUDE)}"\macos\FONTS.H"\
	{$(INCLUDE)}"\macos\MEMORY.H"\
	{$(INCLUDE)}"\macos\MENUS.H"\
	{$(INCLUDE)}"\macos\OSEVENTS.H"\
	{$(INCLUDE)}"\macos\QUICKDRA.H"\
	{$(INCLUDE)}"\macos\RESOURCE.H"\
	{$(INCLUDE)}"\macos\TOOLUTIL.H"\
	{$(INCLUDE)}"\macos\DIALOGS.H"\
	{$(INCLUDE)}"\macos\WINDOWS.H"\
	{$(INCLUDE)}"\macos\MSVCMAC.H"\
	

!IF  "$(CFG)" == "MultSkel - Macintosh Release"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"


"$(INTDIR)\transkel.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"

"$(INTDIR)\vc40.pdb" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\transkel.txt

!IF  "$(CFG)" == "MultSkel - Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Macintosh Debug"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "MultSkel - Power Macintosh Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
