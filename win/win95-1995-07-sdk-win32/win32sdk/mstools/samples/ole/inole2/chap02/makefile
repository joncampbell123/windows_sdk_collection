DIRLIST =   cosmo       \
            patron      \
            skel

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap02\$@ *** >>$(MSTOOLS)\samples\chap02.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap02.tmp
        cd ..

