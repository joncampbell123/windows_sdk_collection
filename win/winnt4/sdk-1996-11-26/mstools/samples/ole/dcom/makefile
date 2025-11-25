
DIRLIST  =  comperf \
            oleimp  \
            simple  \
            secure

all: $(DIRLIST)

$(DIRLIST):
        cd $@
        @echo *** ole\dcom\$@ *** >>$(MSTOOLS)\samples\oledcom.tmp
        @nmake -a -i -nologo >>$(MSTOOLS)\samples\oledcom.tmp
        cd ..
