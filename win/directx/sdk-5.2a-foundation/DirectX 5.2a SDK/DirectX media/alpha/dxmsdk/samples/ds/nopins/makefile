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

# Location relative to root of SDK tree.

!ifndef AXSDK
AXSDK = ..\..\..
!endif

# Target

TARGET_NAME = NoPins
TARGET_TYPE = DYNLINK
TARGET_EXT  = ax

# Source files

SRC_FILES = nopins.cpp
RC_FILE   = nopins.rc
DEF_FILE  = null.def

# DLL specific defines

DLL_BASE  = 0x1dc00000

#define AMOVIE_SELF_REGISTER

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
