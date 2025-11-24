DIRLIST =   patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap09\$@ *** >>$(SDKROOT)\samples\chap09.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap09.tmp
        cd ..

