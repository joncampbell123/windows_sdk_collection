# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\abview\makefile
#
# Copyright (C) 1995 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16EXE
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
OTHERLIBS   = $(OTHERLIBS) mapi.lib ctl3dv2.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapi.lib $(MAPILIB)\ctl3dv2.lib
!ENDIF
!ENDIF

!IF $(WIN32)

BASENAME    = abview32
OTHERLIBS   = uuid.lib
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) nafxcw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) nafxcwd.lib
!ENDIF

!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) mapi32.lib 
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\mapi32.lib 
!ENDIF

!IF $(WIN95)

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

