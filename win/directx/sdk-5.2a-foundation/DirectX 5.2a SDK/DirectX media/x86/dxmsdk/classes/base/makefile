!ifdef NTMAKEENV
!include $(NTMAKEENV)\makefile.def
!else   # NTMAKEENV
#===========================================================================;
#
#  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
#
#---------------------------------------------------------------------------;
#
# ActiveX Project Makefile
#
# Targets are provided by ACTIVEX.MAK
#
# all     Create executables (default).
# clean   Remove all files created by any goal.
#
#===========================================================================;

# don't place

NO_COPY = 1

# NO_ACTIVEX_DEFAULTS = 1 to disable linking to strmbase.lib, etc.

NO_ACTIVEX_DEFAULTS = 1
NO_COMMON_PCH       = 1

# Location relative to root of SDK tree.

!ifndef AXSDK
AXSDK = ..\..
!endif

# Target

!ifdef NODEBUG
TARGET_NAME = StrmBase
!else
TARGET_NAME = StrmBasD
!endif
TARGET_TYPE = LIBRARY

# Include paths - where to find include files (in
# addition to environment INC path)

INC_PATH = .;$(AXSDK)\include

# Source files

SRC_FILES = refclock.cpp  sysclock.cpp  schedule.cpp  \
            amvideo.cpp   ctlutil.cpp   amfilter.cpp  \
            mtype.cpp     outputq.cpp   source.cpp    \
            wxutil.cpp    transip.cpp   wxlist.cpp    \
            winutil.cpp   renbase.cpp   winctrl.cpp   \
            videoctl.cpp  pstream.cpp   pullpin.cpp   \
            transfrm.cpp  vtrans.cpp    \
            cprop.cpp     wxdebug.cpp   combase.cpp   \
            dllentry.cpp  dllsetup.cpp  amextra.cpp   \
            strmctl.cpp   ddmm.cpp      seekpt.cpp

CODE_LIBS = $(AXSDK)\lib\strmiids.lib

# Additional tasks

PCH_HEADER   = streams.h
PCH_OBJ_NAME = _streams

!ifdef NODEBUG
PCH_NAME     = streams
!else
PCH_NAME     = streamsd
!endif

END_TASK     = copy_files

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"

# finally, additional project-specific targets

copy_files:
  if exist $(DBG_RTL)\*.lib copy $(DBG_RTL)\*.lib $(AXSDK)\lib
  if exist $(DBG_RTL)\*.pch copy $(DBG_RTL)\*.pch $(AXSDK)\lib

!endif          # NTMAKEENV
