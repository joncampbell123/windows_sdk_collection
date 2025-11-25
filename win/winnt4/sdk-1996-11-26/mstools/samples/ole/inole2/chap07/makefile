DIRLIST =   cosmo       \
            cocosmo     \
            patron      \
            datatran

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap07\$@ *** >>$(MSTOOLS)\samples\chap07.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap07.tmp
        cd ..

