DIRLIST =   cocosmo       \
            dkoala      \
            ekoala      \
            malloc      \
            objuser     \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap04\$@ *** >>$(SDKROOT)\samples\chap04.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap04.tmp
        cd ..

