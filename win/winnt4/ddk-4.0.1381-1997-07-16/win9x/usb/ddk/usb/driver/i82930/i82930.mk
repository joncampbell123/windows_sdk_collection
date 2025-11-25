#############################################################################
#
#       Microsoft Confidential
#       Copyright (C) Microsoft Corporation 1995
#       All Rights Reserved.
#                                                                          
#       Makefile for I82930 device
#
##########################################################################

ROOT			= ..\..\..\..\..
MINIPORT        = I82930
SRCDIR          = ..
WANT_C1032      = TRUE
DEPENDNAME      = ..\depend.mk
DESCRIPTION     = MINI NT USB Driver
VERDIRLIST	= maxdebug debug retail
LINK32FLAGS = $(LINK32FLAGS) -PDB:none -debugtype:both
!IF "$(VERDIR)" == "debug" || "$(VERDIR)" == "DEBUG"
LIBSNODEP = ..\..\..\LIB\i386\checked\usbd.lib
!ELSE
LIBSNODEP = ..\..\..\LIB\i386\free\usbd.lib
!ENDIF

OBJS = i82930.obj \
	   ioctl.obj \
           ocrw.obj

!include ..\..\..\..\..\dev\master.mk

INCLUDE 	= $(SRCDIR);$(SRCDIR)\..\..\inc;$(INCLUDE)

