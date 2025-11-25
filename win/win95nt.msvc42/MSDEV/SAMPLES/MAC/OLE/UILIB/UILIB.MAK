# Microsoft Visual C++ Generated NMAKE File, Format Version 40000
# ** DO NOT EDIT **

# TARGTYPE "Macintosh Static Library" 0x0304

!IF "$(CFG)" == ""
CFG=Uilib - Macintosh Debug
!MESSAGE No configuration specified.  Defaulting to Uilib - Macintosh\
 ole2uid.lib.
!ENDIF 

!IF "$(CFG)" != "Uilib - Macintosh ole2uir.lib" && "$(CFG)" !=\
 "Uilib - Macintosh Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Uilib.mak" CFG="Uilib - Macintosh Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Uilib - Macintosh Release" (based on "Macintosh Static Library")
!MESSAGE "Uilib - Macintosh Debug" (based on "Macintosh Static Library")
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
# PROP Target_Last_Scanned "Uilib - Macintosh Debug"
CPP=cl.exe

!IF  "$(CFG)" == "Uilib - Macintosh Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir MacRel
# PROP BASE Intermediate_Dir MacRel
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir uilibr
# PROP Intermediate_Dir uilibr
OUTDIR=.\uilibr
INTDIR=.\uilibr

ALL : "$(OUTDIR)\Ole2uir.lib"

CLEAN : 
	-@erase ".\uilibr\Ole2uir.lib"
	-@erase ".\uilibr\msgfiltr.obj"
	-@erase ".\uilibr\regdb.obj"
	-@erase ".\uilibr\Olestd.obj"
	-@erase ".\uilibr\Pastespl.obj"
	-@erase ".\uilibr\insobj.obj"
	-@erase ".\uilibr\utility.obj"
	-@erase ".\uilibr\dbgutil.obj"
	-@erase ".\uilibr\geticon.obj"
	-@erase ".\uilibr\common.obj"
	-@erase ".\uilibr\targtdev.obj"
	-@erase ".\uilibr\links.obj"
	-@erase ".\uilibr\objfdbk.obj"
	-@erase ".\uilibr\busy.obj"
	-@erase ".\uilibr\enumfetc.obj"
	-@erase ".\uilibr\convert.obj"
	-@erase ".\uilibr\ole2ui.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /AL /GX /O2 /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /FR /YX /c
# ADD CPP /nologo /AL /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "NDEBUG" /YX /c
CPP_PROJ=/nologo /AL /GX /O2 /FI"macole\msvcole.h" /D "_MSC_VER" /D "_WINDOWS"\
 /D "_MAC" /D "_68K_" /D "NDEBUG" /Fp"$(INTDIR)/Uilib.pch" /YX /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\uilibr/
CPP_SBRS=
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Uilib.bsc" 
BSC32_SBRS=
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"uilibr\Ole2uir.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/Ole2uir.lib" 
LIB32_OBJS= \
	"$(INTDIR)/msgfiltr.obj" \
	"$(INTDIR)/regdb.obj" \
	"$(INTDIR)/Olestd.obj" \
	"$(INTDIR)/Pastespl.obj" \
	"$(INTDIR)/insobj.obj" \
	"$(INTDIR)/utility.obj" \
	"$(INTDIR)/dbgutil.obj" \
	"$(INTDIR)/geticon.obj" \
	"$(INTDIR)/common.obj" \
	"$(INTDIR)/targtdev.obj" \
	"$(INTDIR)/links.obj" \
	"$(INTDIR)/objfdbk.obj" \
	"$(INTDIR)/busy.obj" \
	"$(INTDIR)/enumfetc.obj" \
	"$(INTDIR)/convert.obj" \
	"$(INTDIR)/ole2ui.obj"

"$(OUTDIR)\Ole2uir.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Uilib - Macintosh Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir MacDebug
# PROP BASE Intermediate_Dir MacDebug
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir uilibd
# PROP Intermediate_Dir uilibd
OUTDIR=.\uilibd
INTDIR=.\uilibd

ALL : "$(OUTDIR)\Ole2uid.lib"

CLEAN : 
	-@erase ".\uilibd\Ole2uid.lib"
	-@erase ".\uilibd\ole2ui.obj"
	-@erase ".\uilibd\Olestd.obj"
	-@erase ".\uilibd\links.obj"
	-@erase ".\uilibd\convert.obj"
	-@erase ".\uilibd\enumfetc.obj"
	-@erase ".\uilibd\insobj.obj"
	-@erase ".\uilibd\msgfiltr.obj"
	-@erase ".\uilibd\Pastespl.obj"
	-@erase ".\uilibd\common.obj"
	-@erase ".\uilibd\utility.obj"
	-@erase ".\uilibd\busy.obj"
	-@erase ".\uilibd\regdb.obj"
	-@erase ".\uilibd\dbgutil.obj"
	-@erase ".\uilibd\geticon.obj"
	-@erase ".\uilibd\objfdbk.obj"
	-@erase ".\uilibd\targtdev.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /AL /Q68m /GX /Z7 /Od /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /FR /YX /c
# ADD CPP /nologo /AL /Q68m /GX /Z7 /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /YX /c
CPP_PROJ=/nologo /AL /Q68m /GX /Z7 /Od /FI"macole\msvcole.h" /D "_MSC_VER" /D\
 "_WINDOWS" /D "_MAC" /D "_68K_" /D "_DEBUG" /Fp"$(INTDIR)/Uilib.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\uilibd/
CPP_SBRS=
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Uilib.bsc" 
BSC32_SBRS=
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"uilibd\Ole2uid.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/Ole2uid.lib" 
LIB32_OBJS= \
	"$(INTDIR)/ole2ui.obj" \
	"$(INTDIR)/Olestd.obj" \
	"$(INTDIR)/links.obj" \
	"$(INTDIR)/convert.obj" \
	"$(INTDIR)/enumfetc.obj" \
	"$(INTDIR)/insobj.obj" \
	"$(INTDIR)/msgfiltr.obj" \
	"$(INTDIR)/Pastespl.obj" \
	"$(INTDIR)/common.obj" \
	"$(INTDIR)/utility.obj" \
	"$(INTDIR)/busy.obj" \
	"$(INTDIR)/regdb.obj" \
	"$(INTDIR)/dbgutil.obj" \
	"$(INTDIR)/geticon.obj" \
	"$(INTDIR)/objfdbk.obj" \
	"$(INTDIR)/targtdev.obj"

"$(OUTDIR)\Ole2uid.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "Uilib - Macintosh Release"
# Name "Uilib - Macintosh Debug"

!IF  "$(CFG)" == "Uilib - Macintosh Release"

!ELSEIF  "$(CFG)" == "Uilib - Macintosh Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Source\convert.c
DEP_CPP_CONVE=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\convert.h"\
	
NODEP_CPP_CONVE=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\convert.obj" : $(SOURCE) $(DEP_CPP_CONVE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\objfdbk.c
DEP_CPP_OBJFD=\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OBJFD=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\objfdbk.obj" : $(SOURCE) $(DEP_CPP_OBJFD) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\common.c
DEP_CPP_COMMO=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_COMMO=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\common.obj" : $(SOURCE) $(DEP_CPP_COMMO) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\busy.c
DEP_CPP_BUSY_=\
	".\..\UILib\Source\common.h"\
	".\..\UILib\Source\uidebug.h"\
	".\..\UILib\Source\busy.h"\
	
NODEP_CPP_BUSY_=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\busy.obj" : $(SOURCE) $(DEP_CPP_BUSY_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\ole2ui.c
DEP_CPP_OLE2U=\
	".\..\UILib\Source\common.h"\
	
NODEP_CPP_OLE2U=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\ole2ui.obj" : $(SOURCE) $(DEP_CPP_OLE2U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\links.obj" : $(SOURCE) $(DEP_CPP_LINKS) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\Olestd.obj" : $(SOURCE) $(DEP_CPP_OLEST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\enumfetc.c
DEP_CPP_ENUMF=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_ENUMF=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\enumfetc.obj" : $(SOURCE) $(DEP_CPP_ENUMF) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\regdb.obj" : $(SOURCE) $(DEP_CPP_REGDB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\Pastespl.obj" : $(SOURCE) $(DEP_CPP_PASTE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\geticon.c
NODEP_CPP_GETIC=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\geticon.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\msgfiltr.c
DEP_CPP_MSGFI=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_MSGFI=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\msgfiltr.obj" : $(SOURCE) $(DEP_CPP_MSGFI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\targtdev.c
DEP_CPP_TARGT=\
	".\..\UILib\Source\uidebug.h"\
	
NODEP_CPP_TARGT=\
	".\..\UILib\Source\OleHeaders.h"\
	

"$(INTDIR)\targtdev.obj" : $(SOURCE) $(DEP_CPP_TARGT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


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
	

"$(INTDIR)\insobj.obj" : $(SOURCE) $(DEP_CPP_INSOB) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Source\dbgutil.c

"$(INTDIR)\dbgutil.obj" : $(SOURCE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
