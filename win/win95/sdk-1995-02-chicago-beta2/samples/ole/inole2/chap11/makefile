DIRLIST =   hcosmo \
            polyline

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap11\$@ *** >>$(SDKROOT)\samples\chap11.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap11.tmp
        cd ..

