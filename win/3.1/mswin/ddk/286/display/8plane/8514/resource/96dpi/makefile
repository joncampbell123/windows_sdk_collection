#   Window's Sample Display Drivers.
#
#   High resolution resources
#   96 DPI

res96dpi: config.bin fonts.bin colortab.bin config4p.bin
   @echo Done!

config.bin:    config.obj
    link config ,config;
    exe2bin config
    erase config.exe

config4p.bin:    config4p.obj
    link config4p ,config4p;
    exe2bin config4p
    erase config4p.exe

config.obj:	config.asm
    masm -T config;

config4p.obj:	config4p.asm
    masm -T config4p;

colortab.bin:  colortab.obj
    link colortab ,colortab;
    exe2bin colortab
    erase colortab.exe

colortab.obj:  colortab.asm
    masm -T colortab;

fonts.bin:	fonts.obj
    link fonts;
    exe2bin fonts
    erase fonts.exe

fonts.obj:	fonts.asm
    masm -T fonts;
