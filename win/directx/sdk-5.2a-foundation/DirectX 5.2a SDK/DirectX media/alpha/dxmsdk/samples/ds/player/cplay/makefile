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

# disable automatic handling of PCH files

DISABLE_PCH = 1

# Location relative to root of SDK tree.

!ifndef AXSDK
AXSDK = ..\..\..\..
!endif

# Target

TARGET_NAME = CPlay
TARGET_TYPE = PROGRAM

# Source files

SRC_FILES = cplay.c   \
            assert.c  \
            about.c   \
            file.c    \
            media.c   \
            toolbar.c

RC_FILE   = player.rc

# Exe specifics

EXE_TYPE = windows

# Dependent Libraries

LINK_LIBS = $(DBG_RTL)\stdwin.obj  \
            shell32.lib            \
            comdlg32.lib

# C defines

C_DEFINES = /Yu"stdwin.h" /Fp$(DBG_RTL)\stdwin.pch

# custom tasks

START_TASK = $(DBG_RTL)\stdwin.obj $(DBG_RTL)\stdwin.pch

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"

# custom targets

$(DBG_RTL)\stdwin.obj $(DBG_RTL)\stdwin.pch : $(INCFILES)
  $(cc) $(cflags) /Yc"stdwin.h" -Fo$(DBG_RTL)\ /Fp$(DBG_RTL)\stdwin.pch $(INC_FLAGS) $(@B).c
