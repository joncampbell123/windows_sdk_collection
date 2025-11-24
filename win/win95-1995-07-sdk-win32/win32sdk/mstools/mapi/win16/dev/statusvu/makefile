#------------------------------------------------------------------------------
# $(MAPITEST)\samples\statusvu\makefile
#
# Copyright (C) 1995 Microsoft Corporation
#------------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16DLLMFC
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\statusvu

RCSOURCE    = statvu.rc
RCDEPS      = $H\resource.h


!IF $(WIN16)
BASENAME    = statvu
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
BASENAME    = statvu32
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

OTHERSUBS   = ..\statusvu\makefile \
              ..\misc\makefile

OBJS    = statvu.obj oper.obj

statvu.obj: \
    $H\statvu.cpp \
    $H\statvu.h

oper.obj: \
    $H\oper.cpp \
    $H\oper.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"
