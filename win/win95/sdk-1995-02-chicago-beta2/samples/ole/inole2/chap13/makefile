DIRLIST =   cosmo       \
            patron      \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap13\$@ *** >>$(SDKROOT)\samples\chap13.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap13.tmp
        cd ..

