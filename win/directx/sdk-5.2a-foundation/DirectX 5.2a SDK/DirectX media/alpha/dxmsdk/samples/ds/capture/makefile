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

TARGET_NAME = amcap
TARGET_TYPE = PROGRAM
EXE_TYPE = windows
TARGET_EXT  = exe

# Source files

SRC_FILES = amcap.cpp status.cpp

RC_FILE   = amcap.rc

DEF_FILE  = amcap.def

!IF EXIST($(MSVCDir)\lib\uuid2.lib)
LINK_LIBS = $(LINK_LIBS) comdlg32.lib msacm32.lib uuid2.lib
!ELSE
LINK_LIBS = $(LINK_LIBS) comdlg32.lib msacm32.lib
!ENDIF
# DLL specific defines

DLL_ENTRY = _DllMainCRTStartup
DLL_BASE  = 0x1c400000

# include ACTIVEX.MAK

!include "$(AXSDK)\tools\activex.mak"
