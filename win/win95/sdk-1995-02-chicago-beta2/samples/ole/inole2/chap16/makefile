DIRLIST =   cosmo       \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap16\$@ *** >>$(SDKROOT)\samples\chap16.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap16.tmp
        cd ..

