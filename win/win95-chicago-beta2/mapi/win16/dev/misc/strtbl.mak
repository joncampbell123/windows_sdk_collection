# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\misc\strtbl.mak
#
# Copyright (C) 1993 Microsoft Corporation
# -----------------------------------------------------------------------------

!INCLUDE    "..\makestuf\xmake1.mak"

H           = $(MAPI)\$(SRCROOT)\misc

OBJS        = strtbl.obj

strtbl.obj: \
  $H\strtbl.c

!INCLUDE    "..\makestuf\xmake2.mak"
