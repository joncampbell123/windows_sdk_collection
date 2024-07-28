res:  8514.res

config.obj:     config.asm
    masm -v-z-t-b63 config;

config.bin:     config.obj
    link config;
    exe2bin config

config4p.bin:     config4p.obj
    link config4p;
    exe2bin config4p

colortab.obj:  colortab.asm
    masm -v-z-t-b63 colortab;

colortab.bin:  colortab.obj
    link colortab;
    exe2bin colortab

fonts.obj:      fonts.asm
    masm -v -z -t -b63 fonts;

fonts.bin:      fonts.obj
    link fonts;
    exe2bin fonts

8514.res:       8514.rc 8514.rcv                                \
		normal.cur  ibeam.cur   wait.cur    cross.cur   \
		up.cur      size.cur    icon.cur    sample.ico  \
		hand.ico    ques.ico    bang.ico    note.ico    \
		config.bin  fonts.bin   colortab.bin config4p.bin \
		combo.bmp   down.bmp    downd.bmp   downi.bmp   \
		left.bmp    leftd.bmp   lefti.bmp   maxd.bmp    \
		min.bmp     mind.bmp    mnarrow.bmp obtncorn.bmp  \
		obtsize.bmp obutton.bmp ocheck.bmp  oclose.bmp   \
		odown.bmp   oleft.bmp   ored.bmp    orest.bmp    \
		oright.bmp  osize.bmp   oup.bmp     ozoom.bmp    \
		restore.bmp restored.bmp right.bmp  rightd.bmp   \
		righti.bmp  sysmenu.bmp  up.bmp     upd.bmp      \
		upi.bmp     max.bmp       
     cd 96dpi
     nmake
     cd ..
     rc -r 8514.rc
