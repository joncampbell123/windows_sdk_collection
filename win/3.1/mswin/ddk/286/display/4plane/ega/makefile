#-----------------------------------------------------------------------
# Microsoft Windows 3.1 EGA Display Driver Makefile
# Copyright (c) 1991 Microsoft Corporation
#-----------------------------------------------------------------------
NAME    = EGA
RESDIR  = ..\rc_med
RESFILE = $(RESDIR)\rc_med.res
MASM    = masm $(MOPT)
LINK    = link $(LOPT)/NOE/NOD/LI/MAP/AL:16
INCS    = -I. -I..
ASM     = $(MASM) $(INCS) $(OPT)

#-----------------------------------------------------------------------
# Options
#-----------------------------------------------------------------------
# MASMFLAGS definitions:
#   EGA    =  0001h		;Build the EGA driver.
#   VGA    =  0002h		;Build the VGA driver.
#   SVGA   =  0004h		;Build the SuperVGA driver.
#   ROM    =  0008h		;Build A ROMable driver.
#   PUBDEFS=  4000h		;Make symbols public.
#   DEBUG  =  8000h		;Build a debug driver.
#-----------------------------------------------------------------------
MOPT = -DMASMFLAGS=04001h -D?QUIET -ML -W2 -T

#-----------------------------------------------------------------------
# Object files
#-----------------------------------------------------------------------
OBJ1 = 3xswitch.obj bitblt.obj blkwhite.obj bmc_eti.obj bmc_ite.obj
OBJ2 = bmc_main.obj buildstr.obj cblt.obj charwdth.obj chkstk.obj
OBJ3 = colorinf.obj colors.obj control.obj trail.obj cursor.obj
OBJ4 = disable.obj discreen.obj lodither.obj dithers.obj
OBJ5 = egainit.obj enable.obj enum.obj fb.obj output.obj
OBJ6 = pixel.obj polyline.obj bstr286.obj
OBJ7 = rlebm.obj robject.obj scanline.obj scanlr.obj
OBJ8 = setmode.obj ssb.obj sswitch.obj strblt.obj sblt286.obj
OBJ9 = egastate.obj ds.obj dss.obj ds286.obj dss286.obj

OBJ = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) $(OBJ6) $(OBJ7) $(OBJ8) $(OBJ9)

#-----------------------------------------------------------------------
# Goals
#-----------------------------------------------------------------------
all:	res $(NAME).rcv $(NAME).drv 

res:
    copy $(NAME).rcv $(RESDIR)\display.rcv
    cd $(RESDIR)
    $(MAKE) 
    cd ..\ega

$(NAME).drv: $(NAME).def $(OBJ) $(RESFILE) ega.obj textout.obj
        $(LINK) @<<
textout.obj+
ega.obj +
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

#-----------------------------------------------------------------------
# Note: The -b option for the RC compiler is an undocumentated switch that
# prevents LOADONCALL FIXED segments from being marked as PRELOAD.
#-----------------------------------------------------------------------

# This goal does not do a complete job of creating the dependancies.
# Some additional text editing will be necessary.
# NOTE: Because of the reference to "includes.exe" THIS GOAL SHOULD 
# NOT BE IN THE DDK VERSION OF THIS MAKEFILE.
depend:
        echo # START Dependencies > depend.txt
        -includes -sjnk -lobj -I.. *.asm >> depend.txt
        -includes -sjnk -lobj -I.. ..\*.asm >> depend.txt
        echo # END Dependencies >> depend.txt

#-----------------------------------------------------------------------
# Dependancies
#-----------------------------------------------------------------------
# START Dependencies 

ega.obj: ega.asm ..\cursor.inc ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\macros.inc
	$(ASM) ega.asm,$@;

3xswitch.obj: ..\3xswitch.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

bitblt.obj: ..\bitblt.asm ..\CLRCONST.INC ..\CLRDATA.INC \
	 ..\CLRLOCAL.INC ..\COMPUTEY.INC ..\COPYDEV.INC ..\CURSOR.INC \
	 ..\DEVCONST.INC ..\DEVLOCAL.INC ..\DISPLAY.INC ..\EGA.INC \
	 ..\EGAMEM.INC ..\EXIT.INC ..\GENCONST.INC ..\GENDATA.INC \
	 ..\GENLOCAL.INC ..\MACROS.INC ..\PATTERN.INC ..\PDEVICE.INC \
	 ..\ROPDEFS.INC ..\ROPTABLE.INC ..\SPECIAL.INC ..\mflags.inc
	$(ASM) ..\$*,$@;

blkwhite.obj: ..\blkwhite.asm
	$(ASM) ..\$*,$@;

bmc_eti.obj: ..\bmc_eti.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

bmc_ite.obj: ..\bmc_ite.asm ..\display.inc ..\lines.inc \
	 ..\macros.inc
	$(ASM) ..\$*,$@;

bmc_main.obj: ..\bmc_main.asm ..\macros.inc
	$(ASM) ..\$*,$@;

buildstr.obj: ..\buildstr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc
	$(ASM) ..\$*,$@;

bstr286.obj: ..\buildstr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc
	$(ASM) -D_286 ..\buildstr.asm,$@;

cblt.obj: ..\cblt.asm ..\CLRCONST.INC ..\CLRLOCAL.INC \
	 ..\DEVCONST.INC ..\DEVDATA.INC ..\DEVLOCAL.INC ..\DISPLAY.INC \
	 ..\EGA.INC ..\EGAMEM.INC ..\GENCONST.INC ..\GENLOCAL.INC \
	 ..\MACROS.INC ..\NJMP.INC ..\ROPDEFS.INC ..\mflags.inc
	$(ASM) ..\$*,$@;

charwdth.obj: ..\charwdth.asm ..\fontseg.inc ..\macros.inc \
	 ..\njmp.inc ..\mflags.inc ..\debug.inc
	$(ASM) ..\$*,$@;

chkstk.obj: ..\chkstk.asm ..\mflags.inc
	$(ASM) ..\$*,$@;

colorinf.obj: ..\colorinf.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

colors.obj: ..\colors.asm
	$(ASM) ..\$*,$@;

control.obj: ..\control.asm ..\cursor.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

cursor.obj: ..\cursor.asm ..\CURSOR.INC ..\mflags.inc
	$(ASM) ..\$*,$@;

disable.obj: ..\disable.asm
	$(ASM) ..\$*,$@;

discreen.obj: ..\discreen.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

lodither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DLORES ..\dither.asm,$@;

hidither.obj: ..\dither.asm ..\mflags.inc
	$(ASM) -DHIRES ..\dither.asm,$@;

dithers.obj: ..\dithers.asm ..\display.inc ..\macros.inc
	$(ASM) ..\$*,$@;

ds.obj: ..\ds.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

ds286.obj: ..\ds.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) -D_286 ..\ds.asm,$@;

dss.obj: ..\dss.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

dss286.obj: ..\dss.asm ..\display.inc ..\ega.inc ..\egamem.inc \
	 ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) -D_286 ..\dss.asm,$@;

egainit.obj: ..\egainit.asm
	$(ASM) ..\$*,$@;

egastate.obj: ..\egastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

enable.obj: ..\enable.asm ..\mflags.inc ..\debug.inc
	$(ASM) ..\$*,$@;

enum.obj: ..\enum.asm ..\macros.inc
	$(ASM) ..\$*,$@;

fb.obj: ..\fb.asm ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

output.obj: ..\output.asm ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

pixel.obj: ..\pixel.asm ..\display.inc ..\drawmod2.inc \
	 ..\ega.inc ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

polyline.obj: ..\polyline.asm ..\clip.inc ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\lines.inc ..\macros.inc \
	 ..\njmp.inc ..\plysolid.inc ..\polybitm.inc ..\polyline.inc \
	 ..\polystyl.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

rlebm.obj: ..\rlebm.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\njmp.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

robject.obj: ..\robject.asm ..\display.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

scanline.obj: ..\scanline.asm ..\display.inc \
	 ..\drawmod2.inc ..\ega.inc ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

scanlr.obj: ..\scanlr.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

setmode.obj: ..\setmode.asm
	$(ASM) ..\$*,$@;

ssb.obj: ..\ssb.asm ..\cursor.inc ..\ega.inc ..\egamem.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

sswitch.obj: ..\sswitch.asm ..\macros.inc ..\debug.inc
	$(ASM) ..\$*,$@;

strblt.obj: ..\strblt.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

sblt286.obj: ..\strblt.asm ..\display.inc ..\ega.inc \
	 ..\egamem.inc ..\fontseg.inc ..\macros.inc ..\strblt.inc ..\mflags.inc
	$(ASM) -D_286 ..\strblt.asm,$@;

textout.obj: ..\textout.asm
	$(ASM) ..\$*,$@;

trail.obj: ..\trail.asm ..\cursor.inc ..\ega.inc \
	 ..\egamemd.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

vgastate.obj: ..\vgastate.asm ..\ega.inc ..\egamem.inc \
	 ..\macros.inc ..\mflags.inc
	$(ASM) ..\$*,$@;

# END Dependencies 
