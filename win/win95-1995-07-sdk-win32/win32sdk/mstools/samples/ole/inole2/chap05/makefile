DIRLIST =   cosmo       \
            cocosmo     \
            fragment    \
            patron      \
            polyline    \
            smasher

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap05\$@ *** >>$(MSTOOLS)\samples\chap05.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap05.tmp
        cd ..

