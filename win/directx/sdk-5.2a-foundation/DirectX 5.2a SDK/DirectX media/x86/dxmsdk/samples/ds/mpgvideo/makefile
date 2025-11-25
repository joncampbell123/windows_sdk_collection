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

DISABLE_PCH = 1

# Location relative to root of SDK tree.

!ifndef AXSDK
AXSDK = ..\..\..
!endif

# Target

TARGET_NAME = MpgVideo
TARGET_TYPE = DYNLINK
TARGET_EXT  = ax

# Source files

SRC_FILES = MpgVideo.cpp Decoder.cpp
RC_FILE   = MpgVideo.rc
DEF_FILE  = MpgVideo.def

# DLL specific defines

DLL_BASE  = 0x1dd80000

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
