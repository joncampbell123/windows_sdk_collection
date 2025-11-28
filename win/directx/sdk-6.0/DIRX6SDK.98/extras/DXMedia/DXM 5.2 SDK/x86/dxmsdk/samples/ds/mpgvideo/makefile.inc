# ==========================================================================;
#
#  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
#
#--------------------------------------------------------------------------;


# NOTE:
# this directory contains a makefile which contains a single line that
# includes the global build process makefile.def. If the
# NTTARGETFILE1 or NTTARGETFILE0 environment
# variable is set then makefile.def includes makefile.inc from the current
# directory. This makefile.inc creates an extra target for nmake to create
# when it is run. NTTARGETFILE0 is built before everything else, and
# NTTARGETFILE1 is built after everything else.

copyfiles:
  @if exist obj\$(TARGET_DIRECTORY)\$(TARGETNAME).dll  \
    copy obj\$(TARGET_DIRECTORY)\$(TARGETNAME).dll     \
         $(SDK_ROOT)\bin\*.*

