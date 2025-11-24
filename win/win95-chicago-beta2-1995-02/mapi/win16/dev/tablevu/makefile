#------------------------------------------------------------------------------
# $(MAPITEST)\samples\tablevu\makefile
#
# Copyright (C) 1994 Microsoft Corporation
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
OTHERLIBS   = $(OTHERLIBS) mapix.lib mapiu.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapix.lib $(MAPILIB)\mapiu.lib
!ENDIF
!ENDIF


!IF $(WIN32)
BASENAME    = tblvu32
OTHERLIBS   = ole32.lib advapi32.lib shell32.lib uuid.lib
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) nafxdw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) nafxdwd.lib
!ENDIF
!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapix32.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapix32.lib 
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

!IF $(CHICAGO)
OBJS    = tblview.obj 
!ENDIF

tblview.obj: \
    $H\tblview.cpp \
    $H\tblview.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"
