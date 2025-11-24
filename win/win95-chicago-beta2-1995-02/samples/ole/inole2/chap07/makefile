DIRLIST =   cosmo       \
            cocosmo     \
            patron      \
            datatran

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap07\$@ *** >>$(SDKROOT)\samples\chap07.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap07.tmp
        cd ..

