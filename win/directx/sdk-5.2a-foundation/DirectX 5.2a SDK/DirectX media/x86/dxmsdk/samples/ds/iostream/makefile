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

TARGET_NAME = SampIOS
TARGET_TYPE = LIBRARY
TARGET_EXT  = lib

# Source files

SRC_FILES = SampIOS.cpp
END_TASK  = copy_header

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"

copy_header:
  @echo Copying header...	
