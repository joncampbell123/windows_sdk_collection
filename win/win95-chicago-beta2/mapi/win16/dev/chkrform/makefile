# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\src\chkrform\makefile
#
# Copyright (C) 1994 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16EXE
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\chkrform

RCSOURCE    = $H\wcheck.rc
RCDEPS      = $H\wcheck.rcv \
              $H\wcheck.ico \
              $H\wchecks.ico \
              $H\piece.cur \
              $H\wcheck.dlg \
              $H\dlg.h

!IF $(WIN16)
BASENAME    = wcheck
OTHERLIBS   = ver mapix mapiu ole2
!ENDIF

!IF $(WIN32)
BASENAME    = wcheck
OTHERLIBS   = version.lib libc.lib kernel32.lib user32.lib gdi32.lib mapix32.lib mapiu32.lib ole32.lib
!ENDIF

OTHERSUBS   = ..\chkrform\makefile

OBJS = engine.obj wcheck.obj form.obj

engine.obj: \
    $H\engine\lut.cpp \
    $H\engine\debugio.cpp \
    $H\engine\quality.cpp \
    $H\engine\prune.cpp \
    $H\engine\check.cpp \
    $H\engine.cpp \
    $H\check.h

wcheck.obj: \
    $H\wcheck.cpp

form.obj: \
    $H\form.cpp

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"


