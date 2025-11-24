DIRLIST =   lnkassis       \
            patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap12\$@ *** >>$(SDKROOT)\samples\chap12.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap12.tmp
        cd ..

