DIRLIST =   cosmo       \
            patron      \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap13\$@ *** >>$(MSTOOLS)\samples\chap13.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap13.tmp
        cd ..

