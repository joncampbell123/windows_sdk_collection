# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\abview\makefile
#
# Copyright (C) 1994 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN32EXE
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\abview

RCSOURCE    = $H\abview.rc
RCDEPS      = $H\resource.h \
              $H\abview.ico \
              $H\abview.h
              
!IF $(WIN16)
BASENAME    = abview
OTHERLIBS   = commdlg.lib compobj.lib shell.lib
DEFS        = $(DEFS) /DWINVER=0x300
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) lafxcw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) lafxcwd.lib
!ENDIF
!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapix.lib ctl3dv2.lib mapiu.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapix.lib $(MAPILIB)\ctl3dv2.lib $(MAPILIB)\mapiu.lib
!ENDIF
!ENDIF

!IF $(WIN32)

BASENAME    = abview32
OTHERLIBS   = ole32.lib advapi32.lib shell32.lib uuid.lib
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) nafxcw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) nafxcwd.lib
!ENDIF

!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapix32.lib 
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapix32.lib 
!ENDIF

!IF $(CHICAGO)

!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS)
!ELSE
OTHERLIBS   = $(OTHERLIBS)
!ENDIF

!ELSE 

!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) ctl3d32.lib 
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\ctl3d32.lib
!ENDIF

!ENDIF

!ENDIF

OTHERSUBS   = ..\abview\makefile \
              ..\misc\makefile

OBJS = abview.obj tbllist.obj

abview.obj: \
    $H\abview.cpp \
    $H\abview.h

tbllist.obj: \
    $H\tbllist.cpp \
    $H\tbllist.h

#------------------------------------------------------------------------------

!INCLUDE    "..\makestuf\xmake2.mak"

