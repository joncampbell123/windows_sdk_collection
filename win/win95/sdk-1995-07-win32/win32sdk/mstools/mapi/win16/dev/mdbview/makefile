# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\mdbview\makefile
#
# Copyright (C) 1995 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16EXE
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\mdbview

RCSOURCE    = $H\mdbview.rc
RCDEPS      = $H\resource.h  \
              $H\mdbview.ico \
              $H\mdbview.h
              
!IF $(WIN16)
BASENAME    = mdbvu
OTHERLIBS   = commdlg.lib compobj.lib shell.lib $(MAPILIB)\xvport.lib
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

BASENAME    = mdbvu32
OTHERLIBS   = ole32.lib advapi32.lib shell32.lib $(MAPILIB)\xvport.lib uuid.lib
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


OTHERSUBS   = ..\mdbview\makefile \
              ..\misc\makefile

OBJS    = mdbview.obj prop.obj oper.obj fld.obj msg.obj attach.obj restrict.obj 

mdbview.obj:       \
    $H\mdbview.cpp \
    $H\mdbview.h   \
    $H\resource.h

prop.obj:         \
    $H\prop.cpp   \
    $H\mdbview.h  \
    $H\resource.h

oper.obj:         \
    $H\oper.cpp   \
    $H\mdbview.h  \
    $H\oper.h     \
    $H\resource.h

fld.obj:          \
    $H\fld.cpp    \
    $H\mdbview.h  \
    $H\fld.h      \
    $H\resource.h

msg.obj:          \
    $H\msg.cpp    \
    $H\msg.h      \
    $H\mdbview.h  \
    $H\resource.h

attach.obj:       \
    $H\attach.cpp \
    $H\attach.h   \
    $H\mdbview.h  \
    $H\resource.h

restrict.obj:          \
    $H\restrict.cpp    \
    $H\restrict.h      \
    $H\mdbview.h  \
    $H\resource.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"
