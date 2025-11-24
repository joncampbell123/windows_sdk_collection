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

#-------------------------------------------------------------
# 
# MINI-DRIVER Make file template
#
# The follwing entries should updated
#-------------------------------------------------------------
#
#-------------------------------------------------------------
# Enter the DRV file name (without extension) after DRVNAME =
#-------------------------------------------------------------
# DRIVER NAME
DRVNAME = CBITMAP

#-------------------------------------------------------------
# Enter the names of all of the resident font file names
# (with extensions) after FONTS =
#-------------------------------------------------------------
FONTS = STANDARD.PFM

#-------------------------------------------------------------
# Enter the names of all of the version resource files
# (with extensions) after RCV =
#-------------------------------------------------------------
RCV =	CBITMAP.RCV

#**********************************************************************
# Set up AFLAGS, CFLAGS and LFLAGS
#**********************************************************************
DEBUG=

!ifdef DEBUG

CFLAGS=-nologo -u -c -Asnw -PLM -G2sw -W3 -Od -Zipe -DDEBUG
LFLAGS=/ALIGN:16/NOD/map/Co

!else

CFLAGS=-nologo -u -c -Asnw -PLM -Gsw -W3 -Oasceob1 -Zpe
LFLAGS=/ALIGN:16/NOD/map

!endif

AFLAGS=-DIS_16 -nologo -W2 -Zd -c -Cx -DMASM6


#**********************************************************************
# Do not edit below this line
#**********************************************************************

TARGET:  $(DRVNAME).DRV

$(DRVNAME).res:  $(DRVNAME).rc $(DRVNAME).GPC $(FONTS) $(RCV)
    rc -r -I. $(DRVNAME).RC

libinit.obj: libinit.asm
    set ML=$(AFLAGS)
    ml -Fo.\libinit.obj libinit.asm		

minidriv.obj: minidriv.c
    set CL=$(CFLAGS) $(NOFUNCS)
    cl -Fo.\minidriv.obj minidriv.c


$(DRVNAME).obj:  $(DRVNAME).c
    set CL=$(CFLAGS)
    cl $(DRVNAME).c

$(DRVNAME).exe:     libinit.obj minidriv.obj $(DRVNAME).obj $(DRVNAME).def
    link $(LFLAGS) @<<
libinit minidriv $(DRVNAME) 
$(DRVNAME).exe
$(DRVNAME).map
sdllcew libw
$(DRVNAME).def
<<
    mapsym $(DRVNAME)

$(DRVNAME).drv:     $(DRVNAME).res $(DRVNAME).exe
    rc -40 -t -I. $(DRVNAME)
    copy $(DRVNAME).exe $(DRVNAME).drv
