DIRLIST =   cosmo       \
            cocosmo     \
            patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap08\$@ *** >>$(MSTOOLS)\samples\chap08.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap08.tmp
        cd ..

