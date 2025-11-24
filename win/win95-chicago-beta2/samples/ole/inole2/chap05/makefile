DIRLIST =   cosmo       \
            cocosmo     \
            fragment    \
            patron      \
            polyline    \
            smasher

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap05\$@ *** >>$(SDKROOT)\samples\chap05.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap05.tmp
        cd ..

