# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Power Macintosh Static Library" 0x0404

!IF "$(CFG)" == ""
CFG=UILibPPC - Power Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to UILibPPC - Power Macintosh\
 Debug.
!ENDIF 

!IF "$(CFG)" != "UILibPPC - Power Macintosh Release" && "$(CFG)" !=\
 "UILibPPC - Power Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "UILibPPC.mak" CFG="UILibPPC - Power Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UILibPPC - Power Macintosh Release" (based on\
 "Power Macintosh Static Library")
!MESSAGE "UILibPPC - Power Macintosh Debug" (based on\
 "Power Macintosh Static Library")
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
# PROP Target_Last_Scanned "UILibPPC - Power Macintosh Debug"
CPP=cl.exe

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

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

ALL : ".\PMacRel\ole2uir.lib" ".\PMacRel\UILibPPC.bsc"

CLEAN : 
	-@erase ".\PMacRel\UILibPPC.bsc"
	-@erase ".\PMacRel\links.sbr"
	-@erase ".\PMacRel\msgfiltr.sbr"
	-@erase ".\PMacRel\listdef.sbr"
	-@erase ".\PMacRel\busy.sbr"
	-@erase ".\PMacRel\Pastespl.sbr"
	-@erase ".\PMacRel\geticon.sbr"
	-@erase ".\PMacRel\dbgutil.sbr"
	-@erase ".\PMacRel\ole2ui.sbr"
	-@erase ".\PMacRel\objfdbk.sbr"
	-@erase ".\PMacRel\Olestd.sbr"
	-@erase ".\PMacRel\regdb.sbr"
	-@erase ".\PMacRel\insobj.sbr"
	-@erase ".\PMacRel\convert.sbr"
	-@erase ".\PMacRel\targtdev.sbr"
	-@erase ".\PMacRel\common.sbr"
	-@erase ".\PMacRel\enumfetc.sbr"
	-@erase ".\PMacRel\utility.sbr"
	-@erase ".\PMacRel\ole2uir.lib"
	-@erase ".\PMacRel\common.obj"
	-@erase ".\PMacRel\enumfetc.obj"
	-@erase ".\PMacRel\utility.obj"
	-@erase ".\PMacRel\links.obj"
	-@erase ".\PMacRel\msgfiltr.obj"
	-@erase ".\PMacRel\listdef.obj"
	-@erase ".\PMacRel\busy.obj"
	-@erase ".\PMacRel\Pastespl.obj"
	-@erase ".\PMacRel\geticon.obj"
	-@erase ".\PMacRel\dbgutil.obj"
	-@erase ".\PMacRel\ole2ui.obj"
	-@erase ".\PMacRel\objfdbk.obj"
	-@erase ".\PMacRel\Olestd.obj"
	-@erase ".\PMacRel\regdb.obj"
	-@erase ".\PMacRel\insobj.obj"
	-@erase ".\PMacRel\convert.obj"
	-@erase ".\PMacRel\targtdev.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /ML /W3 /GX /O2 /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /ML /W3 /GX /O2 /FI"macole\msvcole.h" /D "_WINGS" /D "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_PPCMAC" /D "_M_MPPC_" /D pascal= /D _pascal= /D __pascal= /FR /YX /c
CPP_PROJ=/nologo /Zp2 /ML /W3 /GX /O2 /FI"macole\msvcole.h" /D "_WINGS" /D\
 "_MSC_VER" /D "_MAC" /D "_MPPC_" /D "_PPCMAC" /D "_M_MPPC_" /D pascal= /D\
 _pascal= /D __pascal= /FR"$(INTDIR)/" /Fp"$(INTDIR)/UILibPPC.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\PMacRel/
CPP_SBRS=.\PMacRel/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UILibPPC.bsc" 
BSC32_SBRS= \
	".\PMacRel\links.sbr" \
	".\PMacRel\msgfiltr.sbr" \
	".\PMacRel\listdef.sbr" \
	".\PMacRel\busy.sbr" \
	".\PMacRel\Pastespl.sbr" \
	".\PMacRel\geticon.sbr" \
	".\PMacRel\dbgutil.sbr" \
	".\PMacRel\ole2ui.sbr" \
	".\PMacRel\objfdbk.sbr" \
	".\PMacRel\Olestd.sbr" \
	".\PMacRel\regdb.sbr" \
	".\PMacRel\insobj.sbr" \
	".\PMacRel\convert.sbr" \
	".\PMacRel\targtdev.sbr" \
	".\PMacRel\common.sbr" \
	".\PMacRel\enumfetc.sbr" \
	".\PMacRel\utility.sbr"

".\PMacRel\UILibPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PMacRel\ole2uir.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/ole2uir.lib" 
LIB32_OBJS= \
	".\PMacRel\common.obj" \
	".\PMacRel\enumfetc.obj" \
	".\PMacRel\utility.obj" \
	".\PMacRel\links.obj" \
	".\PMacRel\msgfiltr.obj" \
	".\PMacRel\listdef.obj" \
	".\PMacRel\busy.obj" \
	".\PMacRel\Pastespl.obj" \
	".\PMacRel\geticon.obj" \
	".\PMacRel\dbgutil.obj" \
	".\PMacRel\ole2ui.obj" \
	".\PMacRel\objfdbk.obj" \
	".\PMacRel\Olestd.obj" \
	".\PMacRel\regdb.obj" \
	".\PMacRel\insobj.obj" \
	".\PMacRel\convert.obj" \
	".\PMacRel\targtdev.obj"

".\PMacRel\ole2uir.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

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

ALL : ".\PMacDbg\ole2uid.lib"

CLEAN : 
	-@erase ".\PMacDbg\vc40.pdb"
	-@erase ".\PMacDbg\ole2uid.lib"
	-@erase ".\PMacDbg\convert.obj"
	-@erase ".\PMacDbg\enumfetc.obj"
	-@erase ".\PMacDbg\links.obj"
	-@erase ".\PMacDbg\msgfiltr.obj"
	-@erase ".\PMacDbg\Pastespl.obj"
	-@erase ".\PMacDbg\ole2ui.obj"
	-@erase ".\PMacDbg\utility.obj"
	-@erase ".\PMacDbg\Olestd.obj"
	-@erase ".\PMacDbg\listdef.obj"
	-@erase ".\PMacDbg\insobj.obj"
	-@erase ".\PMacDbg\geticon.obj"
	-@erase ".\PMacDbg\dbgutil.obj"
	-@erase ".\PMacDbg\regdb.obj"
	-@erase ".\PMacDbg\objfdbk.obj"
	-@erase ".\PMacDbg\busy.obj"
	-@erase ".\PMacDbg\targtdev.obj"
	-@erase ".\PMacDbg\common.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)/UILibPPC.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "_MAC" /D "_MPPC_" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /Zp2 /W3 /GX /Zi /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D "_PPCMAC" /D "_M_MPPC" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D pascal= /D _pascal= /D __pascal= /D "_WINGS" /YX /c
CPP_PROJ=/nologo /Zp2 /W3 /GX /Zi /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D\
 "_PPCMAC" /D "_M_MPPC" /D "_MAC" /D "_MPPC_" /D "_DEBUG" /D pascal= /D _pascal=\
 /D __pascal= /D "_WINGS" /Fp"$(INTDIR)/UILibPPC.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\PMacDbg/
CPP_SBRS=
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/UILibPPC.bsc" 
BSC32_SBRS=
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"PMacDbg\ole2uid.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/ole2uid.lib" 
LIB32_OBJS= \
	".\PMacDbg\convert.obj" \
	".\PMacDbg\enumfetc.obj" \
	".\PMacDbg\links.obj" \
	".\PMacDbg\msgfiltr.obj" \
	".\PMacDbg\Pastespl.obj" \
	".\PMacDbg\ole2ui.obj" \
	".\PMacDbg\utility.obj" \
	".\PMacDbg\Olestd.obj" \
	".\PMacDbg\listdef.obj" \
	".\PMacDbg\insobj.obj" \
	".\PMacDbg\geticon.obj" \
	".\PMacDbg\dbgutil.obj" \
	".\PMacDbg\regdb.obj" \
	".\PMacDbg\objfdbk.obj" \
	".\PMacDbg\busy.obj" \
	".\PMacDbg\targtdev.obj" \
	".\PMacDbg\common.obj"

".\PMacDbg\ole2uid.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

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

# Name "UILibPPC - Power Macintosh Release"
# Name "UILibPPC - Power Macintosh Debug"

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\busy.c
DEP_CPP_BUSY_=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\busy.h"\
	
NODEP_CPP_BUSY_=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\busy.obj" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\busy.sbr" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\busy.obj" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\busy.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\common.c
DEP_CPP_COMMO=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_COMMO=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\common.sbr" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\common.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\convert.c
DEP_CPP_CONVE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\convert.h"\
	
NODEP_CPP_CONVE=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\convert.sbr" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\convert.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dbgutil.c

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\dbgutil.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\enumfetc.c
DEP_CPP_ENUMF=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_ENUMF=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\enumfetc.obj" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\enumfetc.sbr" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\enumfetc.obj" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\enumfetc.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\geticon.c
NODEP_CPP_GETIC=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\geticon.sbr" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\geticon.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\insobj.c
DEP_CPP_INSOB=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\insobj.h"\
	
NODEP_CPP_INSOB=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\insobj.obj" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\insobj.sbr" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\insobj.obj" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\insobj.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\links.c
DEP_CPP_LINKS=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_LINKS=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\links.sbr" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\links.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\listdef.c
DEP_CPP_LISTD=\
	".\..\UILib\Source\links.h"\
	".\..\UILib\Source\common.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\listdef.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\listdef.sbr" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\listdef.obj" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_LISTD) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\msgfiltr.c
DEP_CPP_MSGFI=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_MSGFI=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\msgfiltr.obj" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\msgfiltr.sbr" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\msgfiltr.obj" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\objfdbk.c
DEP_CPP_OBJFD=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OBJFD=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\objfdbk.obj" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\objfdbk.sbr" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\objfdbk.obj" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ole2ui.c
DEP_CPP_OLE2U=\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLE2U=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\ole2ui.obj" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\ole2ui.sbr" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\ole2ui.obj" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ole2ui.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Olestd.c
DEP_CPP_OLEST=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLEST=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\Olestd.obj" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\Olestd.sbr" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\Olestd.obj" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\olestd.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\Pastespl.c
DEP_CPP_PASTE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\pastespl.h"\
	
NODEP_CPP_PASTE=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\Pastespl.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\Pastespl.sbr" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\Pastespl.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\pastespl.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\regdb.c
DEP_CPP_REGDB=\
	".\..\UILib\Source\regdb.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_REGDB=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\regdb.obj" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\regdb.sbr" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\regdb.obj" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\regdb.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\targtdev.c
DEP_CPP_TARGT=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_TARGT=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\targtdev.obj" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\targtdev.sbr" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\targtdev.obj" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\uidebug.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\utility.c
DEP_CPP_UTILI=\
	".\..\UILib\Source\olestd.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\utility.h"\
	
NODEP_CPP_UTILI=\
	".\..\UILib\Source\OleHeaders.h"\
	

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacRel\utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(BuildCmds)

".\PMacRel\utility.sbr" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"


BuildCmds= \
	$(CPP) $(CPP_PROJ) $(SOURCE) \
	

".\PMacDbg\utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(BuildCmds)

".\PMacDbg\vc40.pdb" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\utility.h

!IF  "$(CFG)" == "UILibPPC - Power Macintosh Release"

!ELSEIF  "$(CFG)" == "UILibPPC - Power Macintosh Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
