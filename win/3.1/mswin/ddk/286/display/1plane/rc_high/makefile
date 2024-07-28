#   Window's Sample Display Drivers.
#
#   High resolution monochrome resources

rc_high:  blkwhite.res color.res

cconfig.obj:    cconfig.asm
    masm -T cconfig;

bconfig.obj:    bconfig.asm
    masm -T bconfig;

cconfig.bin:    cconfig.obj
    link cconfig ,cconfig;
    exe2bin cconfig
    erase cconfig.exe

bconfig.bin:   bconfig.obj
    link bconfig ,bconfig;
    exe2bin bconfig
    erase bconfig.exe

c_ctab.obj:  c_ctab.asm
    masm -T c_ctab;

c_ctab.bin:  c_ctab.obj
    link c_ctab ,c_ctab;
    exe2bin c_ctab
    erase c_ctab.exe

bw_ctab.obj:  bw_ctab.asm
    masm -T bw_ctab;

bw_ctab.bin:  bw_ctab.obj
    link bw_ctab ,bw_ctab;
    exe2bin bw_ctab
    erase bw_ctab.exe

fonts.obj:	fonts.asm
    masm -T fonts;

fonts.bin:	fonts.obj
    link fonts;
    exe2bin fonts
    erase fonts.exe

color.res:      rc_high.rc   cconfig.bin  c_ctab.bin            \
		normal.cur   ibeam.cur	  wait.cur    cross.cur   \
		up.cur	     size.cur	  icon.cur    sample.ico  \
		hand.ico     ques.ico	  bang.ico    note.ico	  \
                fonts.bin    sizenwse.cur sizenesw.cur sizewe.cur \
                sizens.cur   display.rcv
    copy cconfig.bin config.bin
    copy c_ctab.bin  colortab.bin
    rc -r rc_high.rc
    copy rc_high.res color.res
    erase config.bin
    erase colortab.bin
    erase rc_high.res

blkwhite.res:   rc_high.rc   bconfig.bin  bw_ctab.bin            \
		normal.cur   ibeam.cur	  wait.cur    cross.cur   \
		up.cur	     size.cur	  icon.cur    sample.ico  \
		hand.ico     ques.ico	  bang.ico    note.ico	  \
                fonts.bin    sizenwse.cur sizenesw.cur sizewe.cur \
                sizens.cur   display.rcv
    copy bconfig.bin config.bin
    copy bw_ctab.bin  colortab.bin
    rc -r rc_high.rc
    copy rc_high.res blkwhite.res
    erase config.bin
    erase colortab.bin
    erase rc_high.res
