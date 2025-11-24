DIRLIST  =  idl     \
            engines \
            app

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\mfract\$@ *** >>$(SDKROOT)\samples\mfract.tmp
        @nmake -a -i >>$(SDKROOT)\samples\mfract.tmp
        cd ..
