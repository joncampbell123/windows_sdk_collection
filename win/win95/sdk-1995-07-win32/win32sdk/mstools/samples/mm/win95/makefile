#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993-1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################

DIRLIST =   idfedit   \
            joytoy    \
            midiplyr

all: $(DIRLIST)

$(DIRLIST):
	cd $@
	@echo *** mm\win95\$@ *** >>$(MSTOOLS)\samples\mm95.tmp
	@nmake -a -i -nologo >>$(MSTOOLS)\samples\mm95.tmp
	cd ..
	
