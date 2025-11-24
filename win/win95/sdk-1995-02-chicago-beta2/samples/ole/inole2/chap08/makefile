DIRLIST =   cosmo       \
            cocosmo     \
            patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap08\$@ *** >>$(SDKROOT)\samples\chap08.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap08.tmp
        cd ..

