#   Window's Sample Display Drivers.
#
#   Low resolution resources

rc_low:  blkwhite.res

config.obj:     config.asm
    masm -T config;

config.bin:    config.obj
    link config ,config;
    exe2bin config
    erase config.exe

colortab.obj:  colortab.asm
    masm -T colortab;

colortab.bin:  colortab.obj
    link colortab ,colortab;
    exe2bin colortab
    erase colortab.exe

fonts.obj:	fonts.asm
    masm -T fonts;

fonts.bin:	fonts.obj
    link fonts;
    exe2bin fonts
    erase fonts.exe

blkwhite.res:   rc_low.rc    config.bin   colortab.bin            \
		normal.cur   ibeam.cur	  wait.cur    cross.cur   \
		up.cur	     size.cur	  icon.cur    sample.ico  \
		hand.ico     ques.ico	  bang.ico    note.ico	  \
                fonts.bin    display.rcv  \
                sizenwse.cur sizenesw.cur sizewe.cur  sizens.cur
    rc -r rc_low.rc
    copy rc_low.res blkwhite.res
    erase config.bin
    erase fonts.bin
    erase colortab.bin
    erase rc_low.res
