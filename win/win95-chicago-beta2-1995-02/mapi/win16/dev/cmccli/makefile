# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\src\cmccli\makefile
#
# Copyright (C) 1994 Microsoft Corporation
# -----------------------------------------------------------------------------

# Default target type
!IF "$(PRODUCT)" == ""
PRODUCT=WIN16EXE
!ENDIF

!INCLUDE    "..\makestuf\xmake1.mak"
#------------------------------------------------------------------------------

H           = $(MAPI)\$(SRCROOT)\cmccli

RCSOURCE    = $H\cmccli.rc
RCDEPS      = $H\cmccli.ico \
              $H\cmccli2.ico \
              $H\cmccli.h \
              $H\read.bmp \
              $H\unread.bmp \
              $H\reada.bmp \
              $H\unreada.bmp 

!IF $(WIN16)
BASENAME    = cmccli
OTHERLIBS   = commdlg.lib shell.lib lzexpand.lib
!ENDIF

!IF $(WIN32)
BASENAME    = cmccli32
OTHERLIBS   = ole32.lib shell32.lib 
!ENDIF

OTHERSUBS   = ..\cmccli\makefile \
              ..\misc\pvalloc.mak

OBJS = cmccli.obj

cmccli.obj: \
    $H\cmccli.c \
    $H\cmccli.h

#------------------------------------------------------------------------------
!INCLUDE    "..\makestuf\xmake2.mak"


