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
AXSDK = ..\..\..\..
!endif

# Target

TARGET_NAME = MemFile
TARGET_TYPE = PROGRAM

# Source files

SRC_FILES = memfile.cpp

INC_PATH    = ..\include

# Dependent Libraries

LINK_LIBS = ..\base\$(DBG_RTL)\AsynBase.lib

# Type of executable

EXE_TYPE = console

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
