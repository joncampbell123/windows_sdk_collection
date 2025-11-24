DIRLIST =   cosmo

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap10\$@ *** >>$(SDKROOT)\samples\chap10.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap10.tmp
        cd ..

