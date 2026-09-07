!ifdef NTMAKEENV
all:            # NTMAKEENV
clean:          # NTMAKEENV
!else  # NTMAKEENV
#----------------------------------------------------------------------------
#
#  DirectShow SDK Base Class Makefile
#
#  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#  Copyright (c) 1992 - 1998  Microsoft Corporation.  All Rights Reserved.
#
#----------------------------------------------------------------------------

# Location relative to root of SDK tree.

!ifndef DXMEDIASDK
DXMEDIASDK = ..
!endif

# Location relative to master DSHOWSDK sample makefile.

!ifndef DSHOWSAMPLE
# DSHOWSAMPLE = $(DXMEDIASDK)\samples\multimedia\dshow\src
DSHOWSAMPLE = .
!endif

# Do not copy output files to a different directory.

!ifndef NO_COPY
NO_COPY = 1
!endif

DIR_LIST = base


#----------------------------------------------------------------------------

!include <dshowsdk.mak>
!endif          # NTMAKEENV
