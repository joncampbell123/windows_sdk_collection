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

#   Window's Sample Display Drivers.
#
#   High resolution resources

!ifdef PENWIN
OPT	= -DPENWIN
!endif

target:  rc_high.res

config.obj:	config.asm
    ml /Zm /c config.asm

config.bin:    config.obj
    link config ,config;
    exe2bin config
    erase config.exe

colortab.obj:  colortab.asm
    ml /Zm /c colortab.asm

colortab.bin:  colortab.obj
    link colortab ,colortab;
    exe2bin colortab
    erase colortab.exe

fonts.obj:	fonts.asm
    ml /Zm /c fonts.asm

fonts.bin:	fonts.obj
    link fonts;
    exe2bin fonts
    erase fonts.exe

rc_high.res: rc_high.rc display.rcv config.bin   fonts.bin colortab.bin
    rc $(OPT) -r -v rc_high.rc

