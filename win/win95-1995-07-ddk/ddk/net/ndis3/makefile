#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

build: macs  

clean:
	cd mac\ibmtok
	$(MAKE) clean
	cd ..\sonic
	$(MAKE) clean
	cd ..\ne2000
	$(MAKE) clean
	cd ..\ne3200
	$(MAKE) clean
	cd ..\..

macs:
	cd mac\ne2000
	$(MAKE) all
	cd ..\ne3200
	$(MAKE) all
	cd ..\ibmtok
	$(MAKE) all
	cd ..\sonic
	$(MAKE) all
	cd ..

