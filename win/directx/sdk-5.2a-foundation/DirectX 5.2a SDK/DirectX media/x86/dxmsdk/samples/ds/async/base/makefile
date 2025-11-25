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

TARGET_NAME = AsynBase
TARGET_TYPE = LIBRARY

# Source files

SRC_FILES = asyncio.cpp asyncrdr.cpp

INC_PATH    = ..\include

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
