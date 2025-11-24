DIRLIST =   patron

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap15\$@ *** >>$(SDKROOT)\samples\chap15.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap15.tmp
        cd ..

