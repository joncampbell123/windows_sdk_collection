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

OPT = -DPUBDEFS -T -D?QUIET -DHERCULES


#   Includes:
#
#	Define the include on up the display driver tree for this
#	particular directory.

INCS = -I. -I.. -I..\.. -I..\..\..


#   Define the default assemble command.  This command could actually
#   be overridden from the command line, but shouldn't be.

ASM = masm $(INCS) -v -n -ML $(OPT) 			# MASM 4.01
#   ASM = masm $(INCS) -v -n -ML -I\finc $(OPT) 		# MASM 4.00


#   Define the dependencies

#   Many of the following files reside in previous directories, so the
#   normal inference rule does not apply.

plasma:  1plane bw cgaherc rc_high plasma.drv plasma.sym

1plane:
        cd ..\..\..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET"
        cd bw\cgaherc\plasma

bw:
        cd ..\..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET"
        cd cgaherc\plasma

cgaherc:
        cd ..
        $(MAKE) OPT="-DPUBDEFS -T -D?QUIET -DDEBUG -DHERCULES"
        cd plasma

rc_high:
        copy plasma.rcv ..\..\..\rc_high\display.rcv
        cd ..\..\..\rc_high
        del *.res
        $(MAKE) blkwhite.res
        cd ..\bw\cgaherc\plasma

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

cblt.obj:		     ..\cblt.asm	\
		..\..\..\bitblt\genlocal.blt	\
		..\..\..\bitblt\genconst.blt	\
		..\..\..\bitblt\ropdefs.blt	\
			  ..\..\clrlocal.blt	\
			  ..\..\clrconst.blt	\
			     ..\devlocal.blt	\
			     ..\devconst.blt	\
			     ..\devdata.blt
    $(ASM) ..\$*,$@;

polyline.obj:	..\polyline.asm ..\polyline.inc ..\plylocal.inc
    $(ASM) ..\$*,$@;

polybitm.obj:	..\polybitm.asm ..\polyline.inc ..\plylocal.inc ..\polyline.mac
    $(ASM) ..\$*,$@;

polystyl.obj:	..\polystyl.asm ..\polyline.inc ..\plylocal.inc ..\polyline.mac
    $(ASM) ..\$*,$@;

strblt.obj:	..\strblt.asm   ..\strblt.inc ..\..\..\fontseg.inc
    $(ASM) ..\$*,$@;

buildstr.obj:	..\buildstr.asm ..\strblt.inc ..\..\..\fontseg.inc
    $(ASM) ..\$*,$@;

smartfix.obj:	..\smartfix.asm ..\strblt.inc ..\..\..\fontseg.inc
    $(ASM) ..\$*,$@;

smartpro.obj:	..\smartpro.asm ..\strblt.inc ..\..\..\fontseg.inc
    $(ASM) ..\$*,$@;

cursor.obj:	..\cursor.asm cursor.inc 
    $(ASM) ..\$*,$@;

plasma.obj:	plasma.asm
    $(ASM) $**,$@;

3xswitch.obj:	3xswitch.asm
    $(ASM) $**,$@;

plasma.res: plasma.rcv    ..\..\..\rc_high\blkwhite.res
    copy ..\..\..\rc_high\blkwhite.res plasma.res

plasma.drv: plasma.def plasma.lnk plasma.res plasma.rcv 	\
		       .\plasma.obj	     .\cursor.obj	\
		       .\bitblt.obj	     .\cblt.obj		\
		       .\bmc_main.obj	     .\bmc_eti.obj	\
		       .\bmc_ite.obj	     .\discreen.obj	\
		       .\rlebm.obj                              \
		       .\polyline.obj	     .\polybitm.obj	\
		       .\polystyl.obj	     .\strblt.obj	\
		       .\buildstr.obj	     .\smartpro.obj	\
		       .\smartfix.obj	     .\3xswitch.obj	\
		      ..\scanlr.obj	    ..\pixel.obj	\
		      ..\output.obj	    ..\scanline.obj	\
		      ..\init.obj				\
		   ..\..\robject.obj	 ..\..\colorinf.obj	\
		   ..\..\enum.obj				\
		..\..\..\control.obj  ..\..\..\cursors.obj	\
		..\..\..\inquire.obj  ..\..\..\setmode.obj	\
		..\..\..\hidither.obj ..\..\..\disable.obj	\
		..\..\..\enable.obj   ..\..\..\chkstk.obj	\
		..\..\..\charwdth.obj ..\..\..\fb.obj		\
		..\..\..\sswitch.obj
    link @plasma.lnk
    rc plasma.res plasma.drv

plasma.map:  plasma.drv

plasma.sym: plasma.map plasma.drv
    copy plasma.map display.map
    mapsym display
    copy display.sym plasma.sym
    del display.*
