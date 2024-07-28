#   Window's Sample Display Drivers.
#
#   Requirements:
#
#	MASM 4.01 or greater with the environment variable INCLUDE set to
#	the directories containing CMACROS.INC, GDIDEFS.INC, and WINDEFS.INC.
#
#	MASM 4.00 or greater with the ASM inference definition changed to
#	include the directories containing CMACROS.INC, GDIDEFS.INC, and
#	WINDEFS.INC.

#   Options:
#
#	The command line may define options to MASM by defining the OPT
#	macro.	By defining the OPT parameter in the make file, any
#	possible interaction with an environment definition is avoided.

OPT = -DPUBDEFS -DEGA_MONO -T -D?QUIET


#   Includes:
#
#	Define the include on up the display driver tree for this
#	particular directory.

INCS = -I. -I.. -I..\.. -I..\..\..


#   Define the default assemble command.  This command could actually
#   be overridden from the command line, but shouldn't be.

ASM = masm $(INCS) -v -n -ML $(OPT) 			# MASM 4.01
#   ASM = masm $(INCS) -v -n -ML -I\finc $(OPT) 		# MASM 4.00


#   Define the default inference rules

.asm.obj:
	$(ASM) $**,$@;

#   Define the dependencies

#   The files bitblt.asm and cursor.asm reside in other directories,
#   so the normal inference rule doesn't apply.

egamono:  1plane bw ega rc_med egamono.drv egamono.sym

1plane:
        cd ..\..\..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET"
        cd bw\ega\egamono

bw:
        cd ..\..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET"
        cd ega\egamono

ega:
        cd ..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET -DDEBUG"
        cd egamono

rc_med:
        copy egamono.rcv ..\..\..\rc_med\display.rcv
        cd ..\..\..\rc_med
        del *.res
        $(MAKE) blkwhite.res
        cd ..\bw\ega\egamono

bitblt.obj:	..\..\..\bitblt\bitblt.asm	\
		..\..\..\bitblt\genlocal.blt	\
		..\..\..\bitblt\genconst.blt	\
		..\..\..\bitblt\gendata.blt	\
		..\..\..\bitblt\ropdefs.blt	\
		..\..\..\bitblt\roptable.blt	\
			  ..\..\clrlocal.blt	\
			  ..\..\clrconst.blt	\
			  ..\..\clrdata.blt	\
			  ..\..\pdevice.blt	\
			  ..\..\pattern.blt	\
			  ..\..\copydev.blt	\
			     ..\devlocal.blt	\
			     ..\devconst.blt	\
			     ..\devdata.blt	\
			     ..\computey.blt	\
			     ..\special.blt	\
			     ..\exit.blt	\
				cursor.inc
	$(ASM) ..\..\..\bitblt\$*,$@;

bmc_main.obj:  ..\..\..\bitblt\bmc_main.asm
	$(ASM) ..\..\..\bitblt\$*,$@;

bmc_eti.obj:   ..\..\..\bitblt\bmc_eti.asm
	$(ASM) ..\..\..\bitblt\$*,$@;

bmc_ite.obj:   ..\..\..\bitblt\bmc_ite.asm
	$(ASM) ..\..\..\bitblt\$*,$@;

discreen.obj:   ..\..\..\bitblt\discreen.asm
	$(ASM) ..\..\..\bitblt\$*,$@;

rlebm.obj:   ..\..\..\bitblt\rlebm.asm
	$(ASM) ..\..\..\bitblt\$*,$@;

polyline.obj:	..\polyline.asm ..\polybitm.asm ..\polystyl.asm  \
                ..\lines.inc ..\clip.asm
    $(ASM) ..\$*,$@;

strblt.obj:	..\strblt.asm ..\strblt.inc ..\..\..\fontseg.inc
    $(ASM) ..\$*,$@;

scanline.obj:	..\scanline.asm
    $(ASM) ..\$*,$@;

enable.obj:	..\..\..\enable.asm
    $(ASM) ..\..\..\$*,$@;

cursor.obj:	..\cursor.asm ..\egamemd.inc cursor.inc 
    $(ASM) ..\$*,$@;

3xswitch.obj:	3xswitch.asm

egamono.obj:	egamono.asm

egamono.res:  egamono.rcv  ..\..\..\rc_med\blkwhite.res
    copy ..\..\..\rc_med\blkwhite.res egamono.res

egamono.drv:   egamono.def egamono.lnk egamono.res egamono.rcv \
		       .\egamono.obj	     .\cursor.obj	\
		       .\bitblt.obj	     .\3xswitch.obj	\
                       .\bmc_main.obj        .\bmc_eti.obj      \
                       .\bmc_ite.obj         .\discreen.obj     \
		       .\rlebm.obj		                \
                       .\enable.obj	     .\polyline.obj	\
 		       .\scanline.obj	     .\strblt.obj	\
		      ..\cblt.obj 	    ..\scanlr.obj	\
		      ..\pixel.obj	                	\
		      ..\output.obj	    			\
		      ..\buildstr.obj       ..\smartfix.obj	\
		      ..\smartpro.obj       			\
		      ..\ssb.obj	    ..\egainit.obj	\
		   ..\..\robject.obj	 ..\..\colorinf.obj	\
		   ..\..\enum.obj				\
		..\..\..\control.obj  ..\..\..\cursors.obj	\
		..\..\..\inquire.obj  ..\..\..\setmode.obj	\
		..\..\..\hidither.obj ..\..\..\disable.obj	\
		..\..\..\chkstk.obj				\
		..\..\..\charwdth.obj ..\..\..\fb.obj		\
		..\..\..\sswitch.obj
    link @egamono.lnk
    rc egamono.res egamono.drv

egamono.map:  egamono.drv

egamono.sym:  egamono.map
    copy egamono.map display.map
    mapsym display
    copy display.sym egamono.sym
    del display.*
