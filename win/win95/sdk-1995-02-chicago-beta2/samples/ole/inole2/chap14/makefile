DIRLIST =   cosmo       \
            patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap14\$@ *** >>$(SDKROOT)\samples\chap14.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap14.tmp
        cd ..

