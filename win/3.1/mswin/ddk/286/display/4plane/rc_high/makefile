#   Window's Sample Display Drivers.
#
#   High resolution resources

!ifdef PENWIN
OPT	= -DPENWIN
!endif

target:  rc_high.res

config.obj:	config.asm
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

rc_high.res: rc_high.rc display.rcv config.bin   fonts.bin colortab.bin
    rc $(OPT) -r -v rc_high.rc
