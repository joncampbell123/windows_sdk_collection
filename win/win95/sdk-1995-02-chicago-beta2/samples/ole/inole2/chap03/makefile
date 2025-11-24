DIRLIST =   enumc       \
            enumcpp

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\inole2\chap03\$@ *** >>$(SDKROOT)\samples\chap03.tmp
        @nmake -a -i >>$(SDKROOT)\samples\chap03.tmp
        cd ..

