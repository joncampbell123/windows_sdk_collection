DIRLIST =   cocosmo       \
            dkoala      \
            ekoala      \
            malloc      \
            objuser     \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap04\$@ *** >>$(MSTOOLS)\samples\chap04.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\chap04.tmp
        cd ..

