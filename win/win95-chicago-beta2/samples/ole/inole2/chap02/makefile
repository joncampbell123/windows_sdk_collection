DIRLIST =   cosmo       \
            patron      \
            skel

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap02\$@ *** >>$(SDKROOT)\samples\chap02.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap02.tmp
        cd ..

