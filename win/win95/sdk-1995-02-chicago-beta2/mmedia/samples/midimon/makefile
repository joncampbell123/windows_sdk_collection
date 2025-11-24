#===========================================================================;
#
#   Midimon
#
#
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#
#   You can also set the environment variable DEBUG to NO
#
#   to make a 16-bit build, type the following line:
#	nmake WIN16=TRUE
#   just typing 'nmake' will build a 32-bit build
#
#   You can also set the environment variable WIN16 to TRUE
#
#   This makefile spawns either midimn16.mk or midimn32.mk. 
#
#
#===========================================================================;

!if "$(WIN16)" == "TRUE"
MAKEFILE=midimn16.mk
!else
MAKEFILE=midimn32.mk
!endif

goal:
    $(MAKE) -f $(MAKEFILE)

clean:
    $(MAKE) -f $(MAKEFILE) clean
