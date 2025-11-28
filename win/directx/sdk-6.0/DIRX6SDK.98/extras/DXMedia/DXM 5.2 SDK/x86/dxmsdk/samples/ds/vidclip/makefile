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

TARGET_NAME = vidclip
TARGET_TYPE = PROGRAM
EXE_TYPE = windows
LINK_LIBS = $(AXSDK)\lib\amstrmid.lib comctl32.lib comdlg32.lib

# Source files

SRC_FILES = vidclip.cpp document.cpp settings.cpp
RC_FILE   = vidclip.rc
DEF_FILE  = vidclip.def

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
