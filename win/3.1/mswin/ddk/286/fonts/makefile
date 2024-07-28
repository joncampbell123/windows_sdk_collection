all:    fonts.obj       fixed           system

fonts.obj:  fonts.asm
        masm fonts;

fixed:
        cd fixed
        $(MAKE)
        cd ..

system:
        cd system
        $(MAKE)
        cd ..

