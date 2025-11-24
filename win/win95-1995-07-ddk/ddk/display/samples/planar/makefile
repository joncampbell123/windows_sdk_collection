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

4plane:  vga svga

vga:
        del rc_high\display.rcv
        del rc_high\*.res
        cd vga
        $(MAKE)
	cd ..

svga:
        del rc_high\display.rcv
        del rc_high\*.res
        cd svga
        $(MAKE)
	cd ..
