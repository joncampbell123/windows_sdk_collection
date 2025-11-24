# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\sendapp\makefile
#
# Copyright (C) 1994 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16EXE
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\sendapp

RCSOURCE    = $H\sendmail.rc
RCDEPS      = $H\resource.h \
              $H\send.ico \
              $H\sendmail.h \
              $H\smplmapi.h 
              
              

!IF $(WIN16)
BASENAME    = send
OTHERLIBS   = commdlg.lib compobj.lib shell.lib 
DEFS        = $(DEFS) /DWINVER=0x300
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) lafxcw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) lafxcwd.lib 
!ENDIF
!IF $(DEVTREE)
OTHERLIBS   = $(OTHERLIBS) ctl3dv2.lib mapiu.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) $(MAPILIB)\ctl3dv2.lib $(MAPILIB)\mapiu.lib
!ENDIF
!ENDIF


!IF $(WIN32)
BASENAME    = send32
OTHERLIBS   = ole32.lib advapi32.lib shell32.lib
!IF $(VERSHIP)
OTHERLIBS   = $(OTHERLIBS) nafxcw.lib
!ELSE
OTHERLIBS   = $(OTHERLIBS) nafxcwd.lib
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

OTHERSUBS   = ..\sendapp\makefile \
              ..\misc\pvalloc.mak \
              ..\misc\strtbl.mak

OBJS = sendmail.obj smplmapi.obj

sendmail.obj: \
    $H\sendmail.cpp \
    $H\sendmail.h \
    $H\resource.h   

smplmapi.obj: \
    $H\smplmapi.cpp \
    $H\smplmapi.h 

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"


