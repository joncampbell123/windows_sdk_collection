#------------------------------------------------------------------------------
# $(MAPITEST)\samples\propvu\makefile
#
# Copyright (C) 1995 Microsoft Corporation
#------------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16DLLMFC
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\propvu

RCSOURCE    = propvu.rc
RCDEPS      = $H\resource.h

!IF $(WIN16)
BASENAME    = propvu
OTHERLIBS   = commdlg.lib compobj.lib shell.lib
DEFS        = $(DEFS) /DWINVER=0x300
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) lafxdw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) lafxdwd.lib
!ENDIF
!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapi.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapi.lib
!ENDIF
!ENDIF

!IF $(WIN32)
BASENAME    = propvu32
OTHERLIBS   = ole32.lib advapi32.lib shell32.lib uuid.lib
!IF $(VERSHIP) && "$(CPU)" != "MIPS"
OTHERLIBS   = $(OTHERLIBS) nafxdw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) nafxdwd.lib
!ENDIF
!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapi32.lib 
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapi32.lib
!ENDIF
!ENDIF


OTHERSUBS  = ..\propvu\makefile \
             ..\misc\makefile \

OBJS    = propvu.obj oper.obj bldprop.obj getprop.obj getlist.obj namesids.obj iid.obj results.obj

propvu.obj: \
    $H\propvu.cpp \
    $H\propvu.h

oper.obj: \
    $H\oper.cpp \
    $H\oper.h

bldprop.obj: \
    $H\bldprop.cpp \
    $H\bldprop.h

getprop.obj: \
    $H\getprop.cpp \
    $H\getprop.h

getlist.obj: \
    $H\getlist.cpp \
    $H\getlist.h

namesids.obj: \
    $H\namesids.cpp \
    $H\namesids.h

iid.obj: \
    $H\iid.cpp \
    $H\iid.h

results.obj: \
    $H\results.cpp \
    $H\results.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"
