#------------------------------------------------------------------------------
# $(MAPITEST)\samples\tablevu\makefile
#
# Copyright (C) 1995 Microsoft Corporation
#------------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16DLLMFC
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\tablevu

RCSOURCE    = tblview.rc
RCDEPS      = $H\resource.h


!IF $(WIN16)
BASENAME    = tblvu
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
BASENAME    = tblvu32
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

OTHERSUBS   = ..\tablevu\makefile \
              ..\misc\makefile

!IF $(WIN16)
OBJS    = tblview.obj
!ENDIF

!IF $(WIN32)
OBJS    = tblview.obj 
!ENDIF

!IF $(WIN95)
OBJS    = tblview.obj 
!ENDIF

tblview.obj: \
    $H\tblview.cpp \
    $H\tblview.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"
