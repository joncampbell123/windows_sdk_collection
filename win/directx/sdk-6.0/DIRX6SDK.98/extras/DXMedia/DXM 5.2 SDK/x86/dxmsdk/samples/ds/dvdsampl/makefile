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
# QTZDK Project Makefile
#
# Targets are provided by QTZDK.MAK
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

TARGET_NAME = DVDSampl
TARGET_TYPE = PROGRAM
TARGET_EXT = exe
EXE_TYPE = windows

# Source files

SRC_FILES = DVDSampl.cpp

RC_FILE = DVDSampl.rc

DEF_FILE = DVDSampl.def

INC_PATH    = $(AXSDK)\include;$(AXSDK)\classes\base

# Dependent Libraries
!IF EXIST($(MSVCDir)\lib\uuid2.lib)
LINK_LIBS = $(LINK_LIBS) comdlg32.lib uuid2.lib
!ELSE
LINK_LIBS = $(LINK_LIBS) comdlg32.lib
!ENDIF

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
