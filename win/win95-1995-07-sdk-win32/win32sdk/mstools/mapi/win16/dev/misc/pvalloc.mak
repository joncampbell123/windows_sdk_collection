# -----------------------------------------------------------------------------
# $(MAPITEST)\samples\misc\pvalloc.mak
#
# Copyright (C) 1995 Microsoft Corporation
# -----------------------------------------------------------------------------

!INCLUDE    "..\makestuf\xmake1.mak"

H           = $(MAPI)\$(SRCROOT)\misc

OBJS        = pvalloc.obj

pvalloc.obj: \
  $H\pvalloc.c

!INCLUDE    "..\makestuf\xmake2.mak"
