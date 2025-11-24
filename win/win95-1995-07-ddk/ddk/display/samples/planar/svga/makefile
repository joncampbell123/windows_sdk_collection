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

#-----------------------------------------------------------------------
# SuperVGA (800x600) Display Driver Makefile
#-----------------------------------------------------------------------
# Notes:
# 1) To build a ROM driver, set environment variable ROM=1, delete
#    all object files, and rebuild.
#-----------------------------------------------------------------------

!ifdef ROM
NAME    = SVGAR
!else
NAME    = SUPERVGA
!endif
RESDIR  = ..\rc_high
RESFILE = $(RESDIR)\rc_high.res
MASM    = ml $(MOPT)
LINK    = link $(LOPT)/NOE/NOD/LI/MAP/AL:16
INCS    = -I. -I..
ASM     = $(MASM) $(INCS) $(OPT)

#-----------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------
# MASMFLAGS definitions:
#   EGA    =  0001h             ;Build the EGA driver.
#   VGA    =  0002h             ;Build the VGA driver.
#   SVGA   =  0004h             ;Build the SuperVGA driver.
#   ROM    =  0008h             ;Build A ROMable driver.
#   PUBDEFS=  4000h             ;Make symbols public.
#   DEBUG  =  8000h             ;Build a debug driver.
#-----------------------------------------------------------------------
!ifdef ROM
MOPT = -DMASMFLAGS=0400Eh -D?QUIET /c /Zm /Cp /W2 /nologo
!else
MOPT = -DMASMFLAGS=04006h -D?QUIET /c /Zm /Cp /W2 /nologo
!endif

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = 3xswitch.obj bitblt.obj blkwhite.obj bmc_eti.obj bmc_ite.obj
OBJ2 = bmc_main.obj buildstr.obj cblt.obj charwdth.obj chkstk.obj
OBJ3 = colorinf.obj colors.obj control.obj trail.obj cursor.obj
OBJ4 = disable.obj discreen.obj hidither.obj dithers.obj
OBJ5 = egainit.obj enable.obj enum.obj fb.obj output.obj
OBJ6 = pixel.obj polyline.obj inkready.obj
OBJ7 = rlebm.obj robject.obj scanline.obj scanlr.obj
OBJ8 = setmode.obj ssb.obj sswitch.obj strblt.obj
OBJ9 = vgastate.obj ds.obj dss.obj mode.obj

OBJ = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9)

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------
!ifdef ROM
default:        romres2 $(NAME)2.drv romres3 $(NAME)3.drv 
!else
default:        res $(NAME).rcv $(NAME).drv 
!endif

!ifdef ROM
romres2:
    copy $(NAME)2.rcv $(RESDIR)\display.rcv
    del $(RESFILE)
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

romres3:
    del $(RESFILE)
    copy $(NAME)2.rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

!endif

res: $(RESDIR)\$(NAME).mrk
    copy $(NAME).rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) OPT="-D_SVGA=1"
    cd ..\svga

$(RESDIR)\$(NAME).mrk:
	del $(RESDIR)\*.mrk
	del $(RESDIR)\*.res
	echo $(NAME) > $(RESDIR)\$(NAME).mrk

!ifdef ROM
$(NAME)2.drv: $(NAME)2.def $(OBJ) res svgar2.obj 
	$(LINK) @<<
svgar2.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
$(NAME)2.drv
$(NAME)2.map
libw
$(NAME)2.def
<<
	rc -b $(RESFILE) $(NAME)2.drv
	copy $(NAME)2.map display.map
	-mapsym /n display
	del $(NAME)2.sym
	rename display.sym $(NAME)2.sym

#-----------------------------------------------------------------------
# Note: The -b option for the RC compiler is an undocumentated switch that
# prevents LOADONCALL FIXED segments from being marked as PRELOAD.
#-----------------------------------------------------------------------

$(NAME)3.drv: $(NAME)3.def $(OBJ) res supervga.obj
	$(LINK) @<<
supervga.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
$(NAME)3.drv
$(NAME)3.map
libw
$(NAME)3.def
<<
	rc -b $(RESFILE) $(NAME)3.drv
	copy $(NAME)3.map display.map
	-mapsym /n  display
	del $(NAME)3.sym
	rename display.sym $(NAME)3.sym

!else
$(NAME).drv: $(NAME).def $(OBJ) res supervga.obj  
	$(LINK) @<<
supervga.obj +
$(OBJ1) +
$(OBJ2) +
$(OBJ3) +
$(OBJ4) +
$(OBJ5) +
$(OBJ6) +
$(OBJ7) +
$(OBJ8) +
$(OBJ9) 
$(NAME).drv
$(NAME).map
libw
$(NAME).def
<<
	rc -b $(RESFILE) $(NAME).drv
	copy $(NAME).map display.map
	-mapsym /n  display
	del $(NAME).sym
	rename display.sym $(NAME).sym
!endif

# This goal does not do a complete job of creating the dependancies.
# Some additional text editing will be necessary.
# NOTE: Because of the reference to "includes.exe" THIS GOAL SHOULD 
# NOT BE IN THE DDK VERSION OF THIS MAKEFILE.
depend:
	echo # START Dependencies > depend.txt
	-includes -sjnk -lobj -I.. ..\vga\*.asm >> depend.txt
	-includes -sjnk -lobj -I.. ..\*.asm >> depend.txt
	echo # END Dependencies >> depend.txt

#-----------------------------------------------------------------------
# Dependancies
#-----------------------------------------------------------------------
# START Dependencies 

supervga.obj: ..\vga\vga.asm ..\cursor.inc ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) /Fosupervga.obj ..\vga\vga.asm 

svgar2.obj: ..\vga\vga.asm ..\cursor.inc ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) -DONLY286 /Fosvgar2.obj ..\vga\vga.asm

inkready.obj: ..\inkready.asm
	$(ASM) /Fo$@ ..\$*.asm 

3xswitch.obj: ..\3xswitch.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

bitblt.obj: ..\bitblt.asm ..\CLRCONST.INC ..\CLRDATA.INC \
	 ..\CLRLOCAL.INC ..\COMPUTEY.INC ..\COPYDEV.INC ..\CURSOR.INC \
	 ..\DEVCONST.INC ..\DEVLOCAL.INC ..\DISPLAY.INC ..\EGA.INC \
	 ..\EGAMEM.INC ..\EXIT.INC ..\GENCONST.INC ..\GENDATA.INC \
	 ..\GENLOCAL.INC ..\MACROS.INC ..\PATTERN.INC ..\PDEVICE.INC \
	 ..\ROPDEFS.INC ..\ROPTABLE.INC ..\SPECIAL.INC ..\mflags.inc
	$(ASM) ..\$*.asm

blkwhite.obj: ..\blkwhite.asm
	$(ASM) ..\$*.asm

bmc_eti.obj: ..\bmc_eti.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

bmc_ite.obj: ..\bmc_ite.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc
	$(ASM) ..\$*.asm

bmc_main.obj: ..\bmc_main.asm ..\macros.inc
	$(ASM) ..\$*.asm

buildstr.obj: ..\buildstr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc
	$(ASM) ..\$*.asm

cblt.obj: ..\cblt.asm ..\CLRCONST.INC ..\CLRLOCAL.INC \
	 ..\DEVCONST.INC ..\DEVDATA.INC ..\DEVLOCAL.INC ..\DISPLAY.INC \
	 ..\EGA.INC ..\EGAMEM.INC ..\GENCONST.INC ..\GENLOCAL.INC \
	 ..\MACROS.INC ..\NJMP.INC ..\ROPDEFS.INC ..\mflags.inc
	$(ASM) ..\$*.asm

charwdth.obj: ..\charwdth.asm ..\fontseg.inc ..\macros.inc \
	 ..\njmp.inc ..\debug.inc ..\mflags.inc
	$(ASM) ..\$*.asm

chkstk.obj: ..\chkstk.asm ..\mflags.inc
	$(ASM) ..\$*.asm

colorinf.obj: ..\colorinf.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

colors.obj: ..\colors.asm
	$(ASM) ..\$*.asm

control.obj: ..\control.asm ..\cursor.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

cursor.obj: ..\cursor.asm ..\CURSOR.INC ..\mflags.inc
	$(ASM) ..\$*.asm

disable.obj: ..\disable.asm
	$(ASM) ..\$*.asm

discreen.obj: ..\discreen.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

lodither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DLORES /Fo$@ ..\dither.asm

hidither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DHIRES /Fo$@ ..\dither.asm

dithers.obj: ..\dithers.asm ..\display.inc ..\macros.inc
	$(ASM) ..\$*.asm

ds.obj: ..\ds.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*.asm

dss.obj: ..\dss.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*.asm

egainit.obj: ..\egainit.asm
	$(ASM) ..\$*.asm

egastate.obj: ..\egastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) ..\$*.asm

enable.obj: ..\enable.asm ..\debug.inc
	$(ASM) ..\$*.asm

enum.obj: ..\enum.asm ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

fb.obj: ..\fb.asm ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

output.obj: ..\output.asm ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

pixel.obj: ..\pixel.asm ..\display.inc ..\drawmod2.inc \
	 ..\ega.inc ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

polyline.obj: ..\polyline.asm ..\clip.inc ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\lines.inc ..\macros.inc \
	 ..\njmp.inc ..\plysolid.inc ..\polybitm.inc ..\polyline.inc \
	 ..\polystyl.inc ..\mflags.inc
	$(ASM) ..\$*.asm

rlebm.obj: ..\rlebm.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\njmp.inc ..\mflags.inc
	$(ASM) ..\$*.asm

robject.obj: ..\robject.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

scanline.obj: ..\scanline.asm ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\macros.inc
	$(ASM) ..\$*.asm

scanlr.obj: ..\scanlr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

setmode.obj: ..\setmode.asm
	$(ASM) ..\$*.asm

ssb.obj: ..\ssb.asm ..\cursor.inc ..\ega.inc ..\egamem.inc ..\mflags.inc
	$(ASM) ..\$*.asm

sswitch.obj: ..\sswitch.asm ..\macros.inc ..\debug.inc ..\mflags.inc
	$(ASM) ..\$*.asm

strblt.obj: ..\strblt.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*.asm

trail.obj: ..\trail.asm ..\cursor.inc ..\ega.inc \
	 ..\egamemd.inc ..\mflags.inc
	$(ASM) ..\$*.asm

vgastate.obj: ..\vgastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*.asm

mode.obj: ..\mode.asm
      $(ASM) /Fo$@ ..\$*.asm

# END Dependencies 

