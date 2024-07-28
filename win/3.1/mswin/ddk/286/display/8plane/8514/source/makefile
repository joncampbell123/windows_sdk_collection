DEST = OBJ
MASM = -z-t-n-b63-p
RESDIR=..\RESOURCE

8514.DRV:  res_file $(DEST)\8514.DRV $(DEST)\8514.sym

res_file:
        cd $(RESDIR)
        $(MAKE)
        cd ..\source


$(DEST)\bitblt.obj:    bitblt.asm genconst.blt devconst.blt gendata.blt  \
               roptable.blt genlocal.blt clrlocal.blt devlocal.blt 8514.blt  \
	       pdevice.blt pattern.blt copydev.blt computey.blt ropdefs.blt  \
               macros.mac display.inc
    masm  $(MASM) bitblt,$(DEST)\;

$(DEST)\blt4.obj:   blt4.asm boardblt.inc
    masm  $(MASM)-Ml blt4,$(DEST)\;

$(DEST)\blt4_386.obj:   blt4_386.asm boardblt.inc
    masm  $(MASM)-Ml blt4_386,$(DEST)\;

$(DEST)\blt8.obj:   blt8.asm boardblt.inc
    masm  $(MASM)-Ml blt8,$(DEST)\;

$(DEST)\blt8_386.obj:   blt8_386.asm boardblt.inc
    masm  $(MASM)-Ml blt8_386,$(DEST)\;

$(DEST)\blkmov.obj:   blkmov.asm 8514.inc boardblt.inc
    masm  $(MASM)-Ml blkmov,$(DEST)\;

$(DEST)\blkwri.obj:	blkwri.asm 8514.inc boardblt.inc
    masm  $(MASM)-Ml blkwri,$(DEST)\;

$(DEST)\boardblt.obj:	 boardblt.asm rop.inc 8514.inc boardblt.inc drvpal.inc
    masm  $(MASM)-Ml boardblt,$(DEST)\;

$(DEST)\brush.obj: brush.asm 8514.inc brush.inc
    masm  $(MASM)-Ml brush,$(DEST)\;

$(DEST)\buildstr.obj:	buildstr.asm strmem.inc macros.mac
    masm  $(MASM) buildstr,$(DEST)\;

$(DEST)\cblt.obj:      cblt.asm genconst.blt devconst.blt ropdefs.blt genlocal.blt   \
               clrlocal.blt devlocal.blt macros.mac display.inc
    masm  $(MASM) cblt,$(DEST)\;

$(DEST)\charwdth.obj:	charwdth.asm macros.mac	debug.inc
    masm  $(MASM) charwdth,$(DEST)\;

$(DEST)\checkcur.obj:	checkcur.asm
    masm  $(MASM) checkcur,$(DEST)\;

$(DEST)\chkstk.obj:	chkstk.asm
    masm  $(MASM) chkstk,$(DEST)\;

$(DEST)\colour.obj: colour.asm 8514.inc
    masm  $(MASM)-Ml colour,$(DEST)\;

$(DEST)\control.obj:	control.asm 8514.inc
    masm  $(MASM)-Ml  control,$(DEST)\;

$(DEST)\data.obj:	data.asm palette.dat drvpal.inc oemres.inc debug.inc
    masm  $(MASM)-Ml  data,$(DEST)\;

$(DEST)\dibentry.obj:	dibentry.asm rlebitm.inc
    masm  $(MASM)-Ml  dibentry,$(DEST)\;

$(DEST)\dibframe.obj:	dibframe.asm dibframe.inc gdibitm.inc
    masm  $(MASM) dibframe,$(DEST)\;

$(DEST)\dibstuf4.obj:	dibstuf4.asm dibframe.inc
    masm  $(MASM) dibstuf4,$(DEST)\;

$(DEST)\dibstuff.obj:	dibstuff.asm dibframe.inc
    masm  $(MASM) dibstuff,$(DEST)\;

$(DEST)\dibtoscr.obj:	dibtoscr.asm gdibitm.inc
    masm  $(MASM) dibtoscr,$(DEST)\;

$(DEST)\dither.obj:	dither.asm
    masm  $(MASM)-Ml  dither,$(DEST)\;

$(DEST)\dithers.obj:	dithers.asm
    masm  $(MASM)-Ml  dithers,$(DEST)\;

$(DEST)\drawmode.obj:	drawmode.asm
    masm  $(MASM) drawmode,$(DEST)\;

$(DEST)\enum.obj:	enum.asm 8514.inc
    masm  $(MASM)-Ml  enum,$(DEST)\;

$(DEST)\getfont.obj:	pgetfont.asm 8514.inc strblt.inc
    masm  $(MASM)-Ml -D_RMODE pgetfont,$(DEST)\getfont.obj;

#
# modified 4/5/91 charlkin
#
$(DEST)\init.obj: init.asm palette.dat 8514.inc debug.inc
    masm  $(MASM)-Ml $(DPI_96X96) init,$(DEST)\;

$(DEST)\move.obj:	move.asm 8514.inc
    masm  $(MASM) move,$(DEST)\;

$(DEST)\output.obj:   output.asm 8514.inc output.inc drvpal.inc
    masm  $(MASM)-Ml output,$(DEST)\;

$(DEST)\output4.obj:  output4.asm output.inc brush.inc
    masm  $(MASM)-Ml output4,$(DEST)\;

$(DEST)\output8.obj:  output8.asm output.inc brush.inc
    masm  $(MASM)-Ml output8,$(DEST)\;

$(DEST)\pbldstr.obj:	pbldstr.asm pstrmem.inc macros.mac
    masm  $(MASM) pbldstr,$(DEST)\;

$(DEST)\pgetfont.obj:	pgetfont.asm 8514.inc strblt.inc
    masm  $(MASM)-Ml -D_PMODE pgetfont,$(DEST)\;

$(DEST)\pixel.obj:	 pixel.asm
    masm  $(MASM) pixel,$(DEST)\;

$(DEST)\pixel4.obj:	 pixel4.asm 8514.inc macros.mac
    masm  $(MASM) pixel4,$(DEST)\;

$(DEST)\pixel8.obj:	 pixel8.asm 8514.inc macros.mac
    masm  $(MASM) pixel8,$(DEST)\;

$(DEST)\polyline.obj: polyline.asm 8514.inc output.inc
    masm  $(MASM)-Ml polyline,$(DEST)\;

$(DEST)\polylin4.obj: polymem.asm clip.asm polydraw.asm
    masm  $(MASM)-Ml -D_PLANE_4 polymem,$(DEST)\polylin4.obj;

$(DEST)\polylin8.obj: polymem.asm clip.asm polydraw.asm
    masm  $(MASM)-Ml -D_PLANE_8 polymem,$(DEST)\polylin8.obj;

$(DEST)\psmartpr.obj:	psmartpr.asm pstrmem.inc macros.mac
    masm  $(MASM) psmartpr,$(DEST)\;

$(DEST)\pstrblt.obj:	pstrblt.asm strblt.inc
    masm  $(MASM)-Ml -D_PMODE pstrblt,$(DEST)\;

$(DEST)\pstrmem.obj:	pstrmem.asm pstrmem.inc macros.mac
    masm  $(MASM) pstrmem,$(DEST)\;

$(DEST)\readscrn.obj:	readscrn.asm 8514.inc boardblt.inc
    masm  $(MASM)-Ml readscrn,$(DEST)\;

$(DEST)\rectngle.obj: rectngle.asm output.inc
    masm  $(MASM)-Ml rectngle,$(DEST)\;

$(DEST)\rld.obj: rld.asm
    masm  $(MASM)-Ml rld,$(DEST)\;

$(DEST)\robject.obj:	robject.asm
    masm  $(MASM)-Ml -D_NEW_COLOR_ADD robject,$(DEST)\;

$(DEST)\routines.obj: routines.asm 8514.inc
    masm  $(MASM)-Ml routines,$(DEST)\;

$(DEST)\savescrn.obj:	savescrn.asm 8514.inc
    masm  $(MASM)-Ml savescrn,$(DEST)\;

$(DEST)\scanline.obj: scanline.asm 8514.inc output.inc
    masm  $(MASM)-Ml scanline,$(DEST)\;

$(DEST)\scanlr.obj:    scanlr.asm 8514.inc macros.mac
    masm  $(MASM) scanlr,$(DEST)\;

$(DEST)\set.obj:	set.asm 8514.inc
    masm  $(MASM)-Ml set,$(DEST)\;

$(DEST)\smartpro.obj:	smartpro.asm strmem.inc macros.mac
    masm  $(MASM) smartpro,$(DEST)\;

$(DEST)\sswitch.obj:	sswitch.asm debug.inc
    masm  $(MASM) sswitch,$(DEST)\;

$(DEST)\strblt.obj:	pstrblt.asm strblt.inc
    masm  $(MASM)-Ml -D_RMODE pstrblt,$(DEST)\strblt.obj;

$(DEST)\strcolor.obj:	strcolor.asm strblt.inc drvpal.inc
    masm  $(MASM)-Ml strcolor,$(DEST)\;

$(DEST)\strmem.obj:	strmem.asm strmem.inc macros.mac
    masm  $(MASM) strmem,$(DEST)\;

$(DEST)\stubs.obj:	stubs.asm
    masm  $(MASM)-Ml stubs,$(DEST)\;

$(DEST)\tbinit.obj:	tbinit.asm brush.inc 8514.inc drvpal.inc 8514port.inc
    masm  $(MASM) tbinit,$(DEST)\;

$(DEST)\textout.obj:	textout.asm strblt.inc
    masm  $(MASM)-Ml textout,$(DEST)\;

$(DEST)\8514.DRV:   8514.DEF 8514.LNK $(RESDIR)\8514.res\
	    $(DEST)\sswitch.obj $(DEST)\bitblt.obj $(DEST)\strblt.obj   \
            $(DEST)\dibframe.obj $(DEST)\dibstuff.obj                   \
            $(DEST)\dibtoscr.obj $(DEST)\init.obj $(DEST)\brush.obj     \
            $(DEST)\colour.obj $(DEST)\scanline.obj $(DEST)\output.obj  \
            $(DEST)\polyline.obj $(DEST)\enum.obj $(DEST)\control.obj   \
            $(DEST)\strmem.obj $(DEST)\dither.obj $(DEST)\chkstk.obj    \
            $(DEST)\robject.obj $(DEST)\drawmode.obj                    \
            $(DEST)\rectngle.obj $(DEST)\checkcur.obj                   \
            $(DEST)\boardblt.obj $(DEST)\cblt.obj $(DEST)\data.obj      \
            $(DEST)\blt4.obj $(DEST)\blt8.obj $(DEST)\getfont.obj       \
            $(DEST)\blkwri.obj $(DEST)\blkmov.obj $(DEST)\output4.obj   \
            $(DEST)\output8.obj	$(DEST)\scanlr.obj $(DEST)\routines.obj \
            $(DEST)\readscrn.obj $(DEST)\strcolor.obj                   \
            $(DEST)\dibstuf4.obj $(DEST)\move.obj $(DEST)\set.obj       \
            $(DEST)\pixel.obj $(DEST)\smartpro.obj $(DEST)\pixel4.obj   \
            $(DEST)\pixel8.obj $(DEST)\pstrblt.obj $(DEST)\pgetfont.obj \
            $(DEST)\pstrmem.obj $(DEST)\pbldstr.obj $(DEST)\psmartpr.obj\
            $(DEST)\textout.obj $(DEST)\dibentry.obj $(DEST)\dithers.obj\
            $(DEST)\polylin8.obj $(DEST)\polylin4.obj                   \
            $(DEST)\buildstr.obj $(DEST)\savescrn.obj $(DEST)\stubs.obj \
            $(DEST)\charwdth.obj $(DEST)\tbinit.obj $(DEST)\rld.obj	\
            $(DEST)\blt8_386.obj $(DEST)\blt4_386.obj
        cd $(DEST)
        link @..\8514.LNK
        rc -b ..\$(RESDIR)\8514.res 8514.DRV
        cd ..
#
# Note: The -b option for the RC compiler is an undocumentated switch that
# prevents LOADONCALL FIXED segments from being marked as PRELOAD.
#

$(DEST)\8514.map:  $(DEST)\8514.DRV

$(DEST)\8514.sym:   $(DEST)\8514.map
        cd $(DEST)
        copy 8514.map display.map
        mapsym display
        copy display.sym 8514.sym
        del display.sym
        del display.map
        cd ..

