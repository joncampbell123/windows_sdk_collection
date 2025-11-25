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
USE_LIBCMT_LIB = 1

# Location relative to root of SDK tree.

!ifndef AXSDK
AXSDK = ..\..\..\..
!endif

# Target

TARGET_NAME = MpegProp
TARGET_TYPE = PROGRAM

# Source files

SRC_FILES = mpegprop.cpp

RC_FILE   = mpegprop.rc

# Exe specifics

EXE_TYPE = windows

# Dependent Libraries

LINK_LIBS = shell32.lib comdlg32.lib olepro32.lib comctl32.lib

!include "$(AXSDK)\tools\activex.mak"
