DIRLIST =   cosmo       \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap16\$@ *** >>$(MSTOOLS)\samples\chap16.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap16.tmp
        cd ..

