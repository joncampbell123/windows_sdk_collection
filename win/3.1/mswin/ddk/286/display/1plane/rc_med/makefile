#   Window's Sample Display Drivers.
#
#   High resolution resources

rc_med:  blkwhite.res color.res

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

color.res:      rc_med.rc    cconfig.bin  c_ctab.bin            \
		normal.cur   ibeam.cur	  wait.cur    cross.cur   \
		up.cur	     size.cur	  icon.cur    sample.ico  \
		hand.ico     ques.ico	  bang.ico    note.ico	  \
                fonts.bin    display.rcv \
                sizenwse.cur sizenesw.cur sizewe.cur  sizens.cur
    copy cconfig.bin config.bin
    copy c_ctab.bin  colortab.bin
    rc -r rc_med.rc
    copy rc_med.res color.res
    erase config.bin
    erase colortab.bin
    erase rc_med.res

blkwhite.res:   rc_med.rc    bconfig.bin  bw_ctab.bin            \
		normal.cur   ibeam.cur	  wait.cur    cross.cur   \
		up.cur	     size.cur	  icon.cur    sample.ico  \
		hand.ico     ques.ico	  bang.ico    note.ico	  \
                fonts.bin    display.rcv  \
                sizenwse.cur sizenesw.cur sizewe.cur  sizens.cur
    copy bconfig.bin config.bin
    copy bw_ctab.bin  colortab.bin
    rc -r rc_med.rc
    copy rc_med.res blkwhite.res
    erase config.bin
    erase colortab.bin
    erase rc_med.res
