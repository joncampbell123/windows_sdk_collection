DIRLIST =   lnkassis       \
            patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap12\$@ *** >>$(MSTOOLS)\samples\chap12.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap12.tmp
        cd ..

